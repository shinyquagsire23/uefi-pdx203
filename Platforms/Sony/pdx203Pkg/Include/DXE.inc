  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  INF MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/TzDxe/ScmDxeLA.inf
  INF Binaries/pdx203/QcomPkg/Drivers/TzDxe/TzDxeLA.inf
!endif

  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  INF MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
  INF EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  INF EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf
  INF MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/EnvDxe/EnvDxe.inf
!endif

  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/SmemDxe/SmemDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/ULogDxe/ULogDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/NpaDxe/NpaDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/CmdDbDxe/CmdDbDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/PwrUtilsDxe/PwrUtilsDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/RpmhDxe/RpmhDxe.inf
!if $(USE_CUSTOM_DISPLAY_DRIVER) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/CPRDxe/CPRDxe.inf
!endif
  INF Binaries/pdx203/QcomPkg/Drivers/PdcDxe/PdcDxe.inf
!endif

  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

  # the boot image ramdisk as a block device and the dram the loader's device tree lists (dispatched from APRIORI.inc,
  # right after CpuDxe)
  INF pdx203Pkg/Drivers/Pdx203BootMemoryDxe/Pdx203BootMemoryDxe.inf

  # the xnu loader is part of the firmware, a driver loaded from a volume is refused by the image verification of this build
  INF DarwinBootPkg/Driver/DarwinBoot.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/ClockDxe/ClockDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/VcsDxe/VcsDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/HWIODxe/HWIODxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/I2CDxe/I2CDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/SPIDxe/SPIDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/SdccDxe/SdccDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UFSDxe/UFSDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/FeatureEnablerDxe/FeatureEnablerDxe.inf
!endif

!if $(USE_CUSTOM_DISPLAY_DRIVER) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf
!else
  INF SiliciumPkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf
!endif

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/PmicDxe/PmicDxeLa.inf
  INF Binaries/pdx203/QcomPkg/Drivers/ChargerExDxe/ChargerExDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/QcomChargerDxe/QcomChargerDxeLA.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UsbPwrCtrlDxe/UsbPwrCtrlDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/AdcDxe/AdcDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/TsensDxe/TsensDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/LimitsDxe/LimitsDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/DDRInfoDxe/DDRInfoDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UsbfnDwc3Dxe/UsbfnDwc3Dxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/XhciDxe/XhciDxe.inf
!endif

  INF MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/UsbMsdDxe/UsbMsdDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UsbDeviceDxe/UsbDeviceDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/UsbInitDxe/UsbInitDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/HashDxe/HashDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/CipherDxe/CipherDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/MacDxe/MacDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/RNGDxe/RngDxe.inf
!endif

  # the pmic keys as a console input device, the stock ButtonsDxe is not here
  INF pdx203Pkg/Drivers/Pdx203KeyInputDxe/Pdx203KeyInputDxe.inf

  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf

!if $(PDX203_VENDOR_HW) == 1
  INF Binaries/pdx203/QcomPkg/Drivers/PILProxyDxe/PILProxyDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/SPSSDxe/SPSSDxe.inf
  INF Binaries/pdx203/QcomPkg/Drivers/ShmBridgeDxe/ShmBridgeDxeLA.inf
!endif
