##
#  Copyright (c) 2011 - 2022, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.
#  Copyright (c) 2018, Bingxing Wang. All rights reserved.
#  Copyright (c) Microsoft Corporation.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = pdx203
  PLATFORM_GUID                  = DD33048A-5AFF-49D5-8045-B669691263F9
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/pdx203Pkg
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = RELEASE|DEBUG
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = pdx203Pkg/pdx203.fdf
  USE_CUSTOM_DISPLAY_DRIVER      = 0

  #
  # 0 = mu drivers only. the loader leaves the panel scanning out the splash buffer (kept alive by the pingpong autorefresh kick in
  #     KonaPkg sec), SimpleFbDxe publishes a gop on it, the pmic keys come from Pdx203KeyInputDxe and the boot image ramdisk is a
  #     block device through Pdx203BootMemoryDxe. nothing from the stock firmware is needed.
  # 1 = also load the stock sony uefi drivers (smem, clocks, rpmh, pmic, ufs, usb, charger, ...) from Binaries/pdx203, see
  #     docs/pdx203.md for where they have to come from. not tested, the stock drivers are not in this tree.
  #
  PDX203_VENDOR_HW               = 0

  #
  # 0 = SM8250
  # 1 = SM8250-AB
  # 2 = SM8250-AC
  #
  SOC_TYPE                       = 0

!include KonaPkg/KonaPkg.dsc.inc

!if ($(USE_CUSTOM_DISPLAY_DRIVER) == 1) && ($(PDX203_VENDOR_HW) == 0)
  !error The stock DisplayDxe chain needs PDX203_VENDOR_HW = 1
!endif

[PcdsFixedAtBuild]
  #
  # DDR Memory
  #
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000

  #
  # UEFI Stack
  #
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0x9FF90000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x40000

  #
  # SMBIOS
  #
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemManufacturer|"Sony"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemModel|"Xperia 1 II"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailModel|"pdx203"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailSku|"XQ-AT52"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemBoardModel|"pdx203"

  #
  # Simple Frame Buffer
  #
  # the splash buffer the loader leaves at "Display Reserved" uses the default panel timing (1096x2560, the 4k mode is a second
  # timing), the stride equals the width in bytes. read back on the phone by the ios-sweet shim.
  #
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferWidth|1096
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferHeight|2560
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferColorDepth|32

  #
  # Command Mode Panel
  #
  # pingpong 0 (mdss 0xae00000 + the first qcom,sde-pp-off of the stock dtb) drives dsi0, the loader leaves tearcheck off.
  # vsync count = 19.2 mhz / (vtotal 2584 lines * 60 hz), the values the ios-sweet shim programmed and saw autorefresh take.
  #
  gQcomPkgTokenSpaceGuid.PcdMdpPingPongBase|0x0AE71000
  gQcomPkgTokenSpaceGuid.PcdMdpTearCheckVsyncCount|123

  #
  # Platform PEI
  #
  gQcomPkgTokenSpaceGuid.PcdPlatformType|"LA"
!if $(PDX203_VENDOR_HW) == 0
  # nothing reads the xbl hobs without the stock drivers, and the scan calls into whatever the loader left in the stock fd window
  gQcomPkgTokenSpaceGuid.PcdBuildXblHobs|FALSE
!endif

[LibraryClasses]
  #
  # Memory Libraries
  #
  MemoryMapLib|pdx203Pkg/Library/MemoryMapLib/MemoryMapLib.inf

  #
  # QCOM Libraries
  #
  ConfigurationMapLib|pdx203Pkg/Library/ConfigurationMapLib/ConfigurationMapLib.inf

  #
  # UniXNU Libraries
  #
  UniXNULib|UniXNUPkg/Library/UniXNULib/UniXNULib.inf
  FdtLib|MdePkg/Library/BaseFdtLib/BaseFdtLib.inf

[Components]
  #
  # UniXNU Boot Manager & XNU Loader
  #
  UniXNUPkg/Application/UniXNU/UniXNU.inf
  UniXNUPkg/Driver/UniXNUDxe/UniXNUDxe.inf
  DarwinBootPkg/Driver/DarwinBoot.inf

  #
  # Device Drivers
  #
  pdx203Pkg/Drivers/Pdx203BootMemoryDxe/Pdx203BootMemoryDxe.inf
  pdx203Pkg/Drivers/Pdx203KeyInputDxe/Pdx203KeyInputDxe.inf
  pdx203Pkg/Drivers/Pdx203UsbDxe/Pdx203UsbDxe.inf
