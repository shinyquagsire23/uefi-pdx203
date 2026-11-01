/** @file
  Leave the primary USB controller (DWC3 at 0x0a600000, femto HS PHY at 0x088e3000) ready for the kernel's
  QcomXnuUsbDebug, which takes over an inherited device-mode controller and does not cold-start one.

  Sony's ABL leaves the controller powered (USB30_PRIM GDSC on), in device mode and halted, but some branch
  clocks off (the UTMI clock after a fastboot reboot) and the HS PHY in a state the host does not see D+ from.
  This is the part of ios-sweet's pdx203 bring-up (xnu/patches 0087-0095, verified on this phone) that the
  module does not do itself, run once at DXE time:

    1. every USB30_PRIM branch clock that reads CLK_OFF gets CLK_ENABLE (0087), the pipe clock excepted (0092)
    2. the femto HS PHY init (0089), skipped when EUD is enabled
    3. QSCRATCH: the pipe clock domain from UTMI with PHYSTATUS forced, for high speed only (0094)
    4. GUSB2PHYCFG SUSPHY/ENBLSLPM and GUSB3PIPECTL SUSPHY cleared (0088)
    5. QSCRATCH SS_PHY_CTRL LANE0_PWR_PRESENT (0074)

  The module then does the device soft reset, the device role, DCFG high speed, the QSCRATCH VBUS override, its own
  SMMU context for stream 0 and RUN_STOP. Nothing here touches the GDSC, the controller BCR, the SuperSpeed PHY or
  the SMMU, and no regulator is voted: ios-sweet relies on the bootloader's rails as well.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#define GCC_BASE                  0x00100000ULL
#define GCC_USB30_PRIM_GDSCR      (GCC_BASE + 0xf004)
#define GCC_QUSB2PHY_PRIM_BCR     (GCC_BASE + 0x12000)
#define CBCR_CLK_OFF              BIT31
#define CBCR_CLK_ENABLE           BIT0
#define GDSC_PWR_ON               BIT31

#define EUD_ENABLE                0x088e2000ULL

#define HSPHY_BASE                0x088e3000ULL
#define HSPHY_UTMI_CTRL0          0x3c      /* SLEEPM bit0 */
#define HSPHY_UTMI_CTRL5          0x50      /* POR bit1 */
#define HSPHY_CTRL_COMMON0        0x54      /* FSEL bits 6:4 */
#define HSPHY_CTRL_COMMON1        0x58      /* VBUSVLDEXTSEL0 bit4, PLLBTUNE bit5 */
#define HSPHY_CTRL_COMMON2        0x5c      /* VREGBYPASS bit0 */
#define HSPHY_CTRL1               0x60      /* VBUSVLDEXT0 bit0 */
#define HSPHY_CTRL2               0x64      /* SUSPEND_N bit2, SUSPEND_N_SEL bit3 */
#define HSPHY_PARAM_OVERRIDE      0x70      /* the stock dt's qcom,param-override-seq: 0x43 at 0x70 */
#define HSPHY_CFG0                0x94      /* CMN_CTRL_OVERRIDE_EN bit1 */
#define HSPHY_REFCLK_CTRL         0xa0      /* REFCLK_SEL bits 1:0 */

#define DWC3_BASE                 0x0a600000ULL
#define DWC3_GSNPSID              (DWC3_BASE + 0xc120)
#define DWC3_GCTL                 (DWC3_BASE + 0xc110)
#define DWC3_GUSB2PHYCFG          (DWC3_BASE + 0xc200)
#define DWC3_GUSB3PIPECTL         (DWC3_BASE + 0xc2c0)
#define DWC3_DCTL                 (DWC3_BASE + 0xc704)
#define DWC3_DSTS                 (DWC3_BASE + 0xc70c)

#define QSCRATCH_BASE             0x0a6f8800ULL
#define QSCRATCH_GENERAL_CFG      (QSCRATCH_BASE + 0x08)   /* PIPE_UTMI_CLK_SEL bit0, PIPE3_PHYSTATUS_SW bit3, PIPE_UTMI_CLK_DIS bit8 */
#define QSCRATCH_SS_PHY_CTRL      (QSCRATCH_BASE + 0x30)   /* LANE0_PWR_PRESENT bit24 */

/* the board's USB30_PRIM branches (ios-sweet tools/boards/pdx203.py:98-100): master, sleep, utmi, cfg-noc axi, aggre axi,
   phy aux, phy com aux. the pipe clock (0xf05c) stays off: there is no SuperSpeed PHY running to feed it */
STATIC CONST UINT32 mUsbBranches[] = { 0xf010, 0xf018, 0xf01c, 0xf07c, 0xf080, 0xf054, 0xf058 };

STATIC
VOID
Write32 (
  IN UINT64  Address,
  IN UINT32  Value
  )
{
  MmioWrite32 ((UINTN)Address, Value);
  MemoryFence ();
}

STATIC
VOID
Rmw32 (
  IN UINT64  Address,
  IN UINT32  Mask,
  IN UINT32  Set
  )
{
  Write32 (Address, (MmioRead32 ((UINTN)Address) & ~Mask) | (Set & Mask));
}

STATIC
VOID
EnableBranchClocks (
  VOID
  )
{
  UINTN   Index;
  UINTN   Wait;
  UINT32  Value;

  for (Index = 0; Index < ARRAY_SIZE (mUsbBranches); Index++) {
    Value = MmioRead32 ((UINTN)(GCC_BASE + mUsbBranches[Index]));
    if ((Value & CBCR_CLK_OFF) == 0) {
      continue;
    }

    Write32 (GCC_BASE + mUsbBranches[Index], Value | CBCR_CLK_ENABLE);
    for (Wait = 0; Wait < 5000 && (MmioRead32 ((UINTN)(GCC_BASE + mUsbBranches[Index])) & CBCR_CLK_OFF); Wait++) {
      MicroSecondDelay (1);
    }

    DEBUG ((
      DEBUG_INFO,
      "Pdx203Usb: cbcr 0x%05x 0x%08x -> 0x%08x\n",
      mUsbBranches[Index],
      Value,
      MmioRead32 ((UINTN)(GCC_BASE + mUsbBranches[Index]))
      ));
  }
}

STATIC
VOID
HsPhyInit (
  VOID
  )
{
  /* the PHY in reset, then out, as the linux phy-qcom-snps-femto-v2 driver does */
  Rmw32 (GCC_QUSB2PHY_PRIM_BCR, BIT0, BIT0);
  MicroSecondDelay (150);
  Rmw32 (GCC_QUSB2PHY_PRIM_BCR, BIT0, 0);
  MicroSecondDelay (150);

  Rmw32 (HSPHY_BASE + HSPHY_CFG0, BIT1, BIT1);                 /* CMN_CTRL_OVERRIDE_EN */
  Rmw32 (HSPHY_BASE + HSPHY_UTMI_CTRL5, BIT1, BIT1);           /* POR */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL_COMMON0, 0x70, 0);            /* FSEL = 0 (19.2 MHz) */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL_COMMON1, BIT5, BIT5);         /* PLLBTUNE */
  Rmw32 (HSPHY_BASE + HSPHY_REFCLK_CTRL, 0x3, 0x2);            /* REFCLK_SEL default */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL_COMMON1, BIT4, BIT4);         /* VBUSVLDEXTSEL0 */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL1, BIT0, BIT0);                /* VBUSVLDEXT0 */
  Write32 (HSPHY_BASE + HSPHY_PARAM_OVERRIDE, 0x43);
  Rmw32 (HSPHY_BASE + HSPHY_CTRL_COMMON2, BIT0, BIT0);         /* VREGBYPASS */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL2, BIT3 | BIT2, BIT3 | BIT2);  /* SUSPEND_N_SEL, SUSPEND_N */
  Rmw32 (HSPHY_BASE + HSPHY_UTMI_CTRL0, BIT0, BIT0);           /* SLEEPM */
  Rmw32 (HSPHY_BASE + HSPHY_UTMI_CTRL5, BIT1, 0);              /* POR released */
  Rmw32 (HSPHY_BASE + HSPHY_CTRL2, BIT3, 0);                   /* SUSPEND_N_SEL released */
  Rmw32 (HSPHY_BASE + HSPHY_CFG0, BIT1, 0);                    /* CMN_CTRL_OVERRIDE_EN released */
}

STATIC
VOID
HighSpeedOnlyPipe (
  VOID
  )
{
  UINT32  Value;

  /* as dwc3-msm does without a SuperSpeed PHY: the pipe clock domain runs from UTMI and PHYSTATUS is forced, so
     endpoint commands complete with the SS PHY (and its pipe clock) down */
  Value = MmioRead32 ((UINTN)QSCRATCH_GENERAL_CFG);
  Write32 (QSCRATCH_GENERAL_CFG, Value | BIT8);
  MicroSecondDelay (5);
  Write32 (QSCRATCH_GENERAL_CFG, MmioRead32 ((UINTN)QSCRATCH_GENERAL_CFG) | BIT3 | BIT0);
  MicroSecondDelay (5);
  Write32 (QSCRATCH_GENERAL_CFG, MmioRead32 ((UINTN)QSCRATCH_GENERAL_CFG) & ~BIT8);
}

EFI_STATUS
EFIAPI
Pdx203UsbDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT32  Gdsc;
  UINT32  Id;

  Gdsc = MmioRead32 ((UINTN)GCC_USB30_PRIM_GDSCR);
  if ((Gdsc & GDSC_PWR_ON) == 0) {
    /* nothing to inherit: the module refuses a powered-down controller and says so (QusbPowerDomainOff) */
    DEBUG ((DEBUG_WARN, "Pdx203Usb: usb30_prim gdsc off (0x%08x), controller left alone\n", Gdsc));
    return EFI_SUCCESS;
  }

  EnableBranchClocks ();

  /* the master and cfg-noc clocks must run before the core's registers can be read */
  if ((MmioRead32 ((UINTN)(GCC_BASE + 0xf010)) & CBCR_CLK_OFF) || (MmioRead32 ((UINTN)(GCC_BASE + 0xf07c)) & CBCR_CLK_OFF)) {
    DEBUG ((DEBUG_WARN, "Pdx203Usb: master or cfg-noc clock still off, controller left alone\n"));
    return EFI_SUCCESS;
  }

  Id = MmioRead32 ((UINTN)DWC3_GSNPSID);
  if (((Id & 0xffff0000) != 0x55330000) && ((Id & 0xffff0000) != 0x33310000)) {
    DEBUG ((DEBUG_WARN, "Pdx203Usb: no dwc3 at 0x%lx (id 0x%08x)\n", DWC3_BASE, Id));
    return EFI_SUCCESS;
  }

  if (MmioRead32 ((UINTN)EUD_ENABLE) == 0) {
    HsPhyInit ();
  } else {
    DEBUG ((DEBUG_INFO, "Pdx203Usb: EUD enabled, HS PHY left alone\n"));
  }

  HighSpeedOnlyPipe ();

  /* endpoint commands do not complete while the PHYs may sleep, and ABL leaves ENBLSLPM set */
  Write32 (DWC3_GUSB2PHYCFG, MmioRead32 ((UINTN)DWC3_GUSB2PHYCFG) & ~(BIT8 | BIT6));
  Write32 (DWC3_GUSB3PIPECTL, MmioRead32 ((UINTN)DWC3_GUSB3PIPECTL) & ~BIT17);

  Write32 (QSCRATCH_SS_PHY_CTRL, MmioRead32 ((UINTN)QSCRATCH_SS_PHY_CTRL) | BIT24);

  DEBUG ((
    DEBUG_INFO,
    "Pdx203Usb: ready gdsc 0x%08x id 0x%08x gctl 0x%08x dctl 0x%08x dsts 0x%08x\n",
    Gdsc,
    Id,
    MmioRead32 ((UINTN)DWC3_GCTL),
    MmioRead32 ((UINTN)DWC3_DCTL),
    MmioRead32 ((UINTN)DWC3_DSTS)
    ));
  return EFI_SUCCESS;
}
