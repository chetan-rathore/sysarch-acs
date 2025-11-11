## Table of Contents

- [Base System Architecture](#base-system-architecture)
- [BSA - Architecture Compliance Suite](#bsa---architecture-compliance-suite)
- [Release details](#release-details)
- [Documentation & Guides](#documentation-and-guides)
- [BSA build steps](#acs-build-steps)
  - [UEFI Shell application](#uefi-shell-application)
  - [Linux application](#linux-application)
  - [Baremetal application](#baremetal-application)
- [BSA run steps](#bsa-run-steps)
  - [On UEFI shell](#on-uefi-shell)
  - [On Linux enviroment](#on-linux-enviroment)
- [Limitations](#limitations)
- [License](#license)


## Base System Architecture
**Base System Architecture (BSA)** describes a 64-bit Arm-based hardware system architecture relied upon by operating systems, hypervisors, and firmware. It addresses PE features and key aspects of system architecture.

For more information, download the [BSA specification](https://developer.arm.com/documentation/den0094/latest/)

## BSA - Architecture Compliance Suite

The BSA **Architecture Compliance Suite (ACS)** is a collection of self-checking, portable C-based tests.
This suite provides examples of the invariant behaviors defined in the BSA specification, enabling verification that these behaviors have been implemented and interpreted correctly.

Most tests run from the UEFI (Unified Extensible Firmware Interface) Shell via the BSA UEFI shell application.
A subset runs from Linux via a BSA ACS user-space application and its kernel module.
Tests can also run in a bare-metal environment. Initialization of the bare-metal environment is platform-specific and out of scope for this document.

## Release details
- **Latest release version:** v1.1.2 
- **Execution levels:** Pre-Silicon and Silicon.  
- **Scope:** The compliance suite is **not** a substitute for design verification.  
- **Access to logs:** Arm licensees can contact Arm through their partner managers.  

### BSA ACS version mapping

|   BSA ACS Version   |      BSA Tag ID     | BSA Spec Version |    Pre-Si Support |
|:-------------------:|:-------------------:|:----------------:|------------------:|
|        v1.1.2       |   v25.10_BSA_1.1.2  |   BSA v1.1       |       Yes         |
|        v1.1.0       |   v25.03_REL1.1.0   |   BSA v1.1       |       Yes         |
|        v1.0.9       |   v24.11_REL1.0.9   |   BSA v1.1       |       Yes         |
|        v1.0.8       |   v24.03_REL1.0.8   |   BSA v1.0(c)    |       Yes         |
|        v1.0.7       |   v23.12_REL1.0.7   |   BSA v1.0(c)    |       Yes         |
|        v1.0.6       |v23.11_BootFramework |   BSA v1.0(c)    |       Yes         |
|        v1.0.6       |   v23.09_REL1.0.6   |   BSA v1.0(c)    |       Yes         |
|        v1.0.5       |   v23.07_REL1.0.5   |   BSA v1.0(c)    |       Yes         |
|        v1.0.4       |   v23.03_REL1.0.4   |   BSA v1.0(c)    |       Yes         |
|        v1.0.3       |   v23.01_REL1.0.3   |   BSA v1.0       |       No          |
|        v1.0.2       |   v22.10_REL1.0.2   |   BSA v1.0       |       No          |
|        v1.0.1       |   v22.06_REL1.0.1   |   BSA v1.0       |       No          |
|        v1.0         |   v21.09_REL1.0     |   BSA v1.0       |       No          |

### GitHub branch
- To pick up the release version of the code, check out the corresponding **tag** from the **main** branch.
- To get the latest code with bug fixes and new features, use the **main** branch.

### Prebuilt release binaries
Prebuilt images for each release are available in the [`prebuilt_images`](prebuilt_images/BSA) folder of the main branch.  

## Documentation and Guides
- [Arm BSA Test Scenario Document](docs/arm_bsa_architecture_compliance_test_scenario.pdf) — algorithms for implementable rules and notes on unimplemented rules.  
- [Arm BSA Test Checklist](docs/arm_bsa_testcase_checklist.rst) — test categories (UEFI, Linux, Bare-metal) and applicable systems (IR, ES, SR, Pre-Silicon).  
- [Arm BSA Validation Methodology](docs/arm_bsa_architecture_compliance_validation_methodology.pdf).  
- [Arm BSA ACS User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf).  
- Bare-metal porting guides  
  - [Arm BSA ACS Bare-metal User Guide](docs/arm_bsa_architecture_compliance_bare-metal_user_guide.pdf)  
  - [Bare-metal Code](pal/baremetal/)  

> **Note:** The Bare-metal PCIe enumeration code provided with BSA ACS must be used and **must not be replaced**. It is essential for accurate analysis of test results.

- Exerciser VIP guides - The **Exerciser** is a client device wrapped by a PCIe Endpoint, created to satisfy BSA requirements for PCIe capability validation. Running Exerciser tests increases platform coverage.
  - [Exerciser.md](docs/PCIe_Exerciser/Exerciser.md)
  - [Exerciser_API_porting_guide.md](docs/PCIe_Exerciser/Exerciser_API_porting_guide.md).

## BSA build steps

### UEFI Shell application

#### Prerequisites

- A mainstream Linux distribution on x86 or AArch64.
- Bash Shell for build
- Clone the [EDK2 tree](https://github.com/tianocore/edk2). Recommended commit: `836942fbadb629050b866a8052e6af755bcdf623`.
- Clone the [EDK2 port of libc](https://github.com/tianocore/edk2-libc) into local `<edk2_path>`.
- Install **GCC-ARM 13.2** [toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
- Install prerequisite packages to build EDK2.  
  *Note: Package details are beyond the scope of this document.*

#### To start the ACS build for platform using ACPI table, perform the following steps:

1. `cd <local_edk2_path>`  
2. `git submodule update --init --recursive`  
3. `git clone https://github.com/ARM-software/bsa-acs.git ShellPkg/Application/bsa-acs`  
4. Add the following to the **[LibraryClasses.common]** section in `ShellPkg/ShellPkg.dsc`:
   ```
   BsaValLib|ShellPkg/Application/bsa-acs/val/BsaValLib.inf
   BsaPalLib|ShellPkg/Application/bsa-acs/pal/uefi_acpi/BsaPalLib.inf
   ```
5. Add the following in the **[components]** section of `ShellPkg/ShellPkg.dsc`:
   ```
   ShellPkg/Application/bsa-acs/uefi_app/BsaAcs.inf
   ```

#### To start the ACS build for platform using Device tree, perform the following steps:

1. `cd <local_edk2_path>`  
2. `git submodule update --init --recursive`  
3. `git clone https://github.com/ARM-software/bsa-acs.git ShellPkg/Application/bsa-acs`  
4. Add the following to the **[LibraryClasses.common]** section in `ShellPkg/ShellPkg.dsc`:
   ```
   FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf
   BsaValLib|ShellPkg/Application/bsa-acs/val/BsaValLib.inf
   BsaPalLib|ShellPkg/Application/bsa-acs/pal/uefi_dt/BsaPalLib.inf
   ```
5. Add the following in the **[components]** section of `ShellPkg/ShellPkg.dsc`:
   ```
   ShellPkg/Application/bsa-acs/uefi_app/BsaAcs.inf
   ```

6. Apply the following change in `edk2/MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.c`:
   ```diff
   -Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
   -ASSERT_EFI_ERROR (Status);
   +//Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
   +//ASSERT_EFI_ERROR (Status);
   ```

#### Perform the following steps:
1. `export GCC49_AARCH64_PREFIX=<GCC 13.2 toolchain path>/bin/aarch64-linux-gnu-` (on x86 hosts)  
    For an AArch64 host build, this should point to `/usr/bin/`.
2. `export PACKAGES_PATH=<path to edk2-libc>`  
3. `source edksetup.sh`  
4. `make -C BaseTools/Source/C`  
5. `source ShellPkg/Application/bsa-acs/tools/scripts/acsbuild.sh`

#### Build output
The EFI executable is generated at: `<edk2_path>/Build/Shell/DEBUG_GCC49/AARCH64/Bsa.efi`

### Linux application
Certain Peripheral, PCIe, and Memory Map tests require a Linux OS. This section covers building and executing these tests from the Linux application.

#### Prerequisites
- AArch64 Linux build environment.
- Bash Shell 
- The `build.sh` script supports native builds and cross-compilation.
  - **Native builds:** run directly on the target machine.
  - **Cross-compilation:** pass the Linux version and GCC tool version as script arguments.

#### Perform the following steps
```sh
wget https://gitlab.arm.com/linux-arm/linux-acs/-/raw/master/acs-drv/files/build.sh
chmod +x build.sh
source build.sh
```
- Build Script arguments
  The following arguments can be used with `build.sh`:

 - `-v` or `--version` — Linux kernel version for cross-compilation. Default: **6.8**.  
 - `--GCC_TOOLS` — GCC toolchain version for cross-compilation. Default: **13.2.rel1**.  
 - `--help` — Displays environment info, defaults, usage, and notes.  
 - `--clean` — Removes the `build/` output folder (modules and apps).  
 - `--clean_all` — Removes all downloaded repositories and build artifacts, including the output directory.

#### Build Output
An output folder `build/` is created containing:
 - `bsa_acs.ko`
 - `bsa_app`

##### BSA Linux application build
```sh
cd <bsa-acs path>/linux_app/bsa-acs-app
export CROSS_COMPILE=<ARM64 toolchain path>/bin/aarch64-linux-gnu-
make
```

### Baremetal application
The bare-metal build environment is platform-specific.
As a baseline, steps to integrate and run the bare-metal tests from UEFI Shell are provided in [README.md](pal/baremetal/target/RDN2/README.md).
For details on generating binaries for bare-metal environments, refer to [README.md](pal/baremetal/README.md).


## BSA run steps

### On UEFI shell 

#### Prerequisites
If the system supports **LPIs** (Interrupt ID > 8192), firmware must support handler installation for LPI interrupts.

- For **edk2**, modify the ArmGic driver in `ArmPkg` to support LPI handler installation:
  - In `edk2/ArmPkg/Drivers/ArmGic/GicV3/ArmGicV3Dxe.c`  
    After:
    ```c
    #define ARM_GIC_DEFAULT_PRIORITY  0x80
    ```
    add:
    ```c
    #define ARM_GIC_MAX_NUM_INTERRUPT 16384
    ```
    In `GicV3DxeInitialize` change:
    ```c
    mGicNumInterrupts = ArmGicGetMaxNumInterrupts (mGicDistributorBase);
    ```
    to:
    ```c
    mGicNumInterrupts = ARM_GIC_MAX_NUM_INTERRUPT;
    ```

The execution steps vary by environment. The following assume invocation via the **ACS UEFI shell application**.

#### Silicon
On a system with a functional USB port:

**For DT Systems**
1. Copy `Bsa.efi` and `Shell.efi` to a USB flash drive.  
2. Boot to the **U-Boot** shell.  
3. Insert the USB drive.  
4. Determine the USB device with:
   ```sh
   usb start
   ```
5. Load `Shell.efi` to memory and boot UEFI Shell:
   ```sh
   # Syntax
   fatload usb <dev_num> ${kernel_addr_r} Shell.efi
   # Example
   fatload usb 0 ${kernel_addr_r} Shell.efi
   ```
6. In UEFI Shell, refresh mappings:
   ```sh
   map -r
   ```
7. Change to the USB filesystem (e.g., `fs0:`).  
8. Run `Bsa.efi` with appropriate parameters.  
9. Capture UART console output to a log file.

*Note:* `Shell.efi` is available in `prebuilt_images/IR` (under `prebuilt_images`).

#### Emulation environment with secondary storage
1. Create an image containing `Bsa.efi`, for example:
   ```sh
   mkfs.vfat -C -n HD0 hda.img 2097152
   sudo mount -o rw,loop=/dev/loop0,uid=$(whoami),gid=$(whoami) hda.img /mnt/bsa
   sudo cp "<path to application>/Bsa.efi" /mnt/bsa/
   sudo umount /mnt/bsa
   ```
   *(If `/dev/loop0` is busy, select a free loop device.)*
2. Load the image to secondary storage via a backdoor (environment-specific).  
3. Boot to UEFI Shell.  
4. Identify the filesystem with `map -r`.  
5. Switch to the filesystem (`fs<x>:`).  
6. Run `Bsa.efi` with parameters.  
7. Save UART console output for analysis/certification.

- For application parameters, see the [User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf).

#### Emulation environment without secondary storage
1. Add the path to `Bsa.efi` in the UEFI FD file.  
2. Build a UEFI image including **UEFI Shell**.  
3. Boot to UEFI Shell.  
4. Run `Bsa.efi` to start tests. (See the [User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf) for parameters.)  
5. Capture UART logs for analysis and certification.

### On Linux enviroment

 - Load the BSA kernel module
```sh
sudo insmod bsa_acs.ko
```

 - Run the BSA application
```sh
./bsa_app
# or
./bsa
```
 - Remove the BSA kernel module after run
```sh
sudo rmmod bsa_acs
```

- For application parameters, see the [User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf).

## Limitations
- For systems with firmware compliant to SBBR, BSA depends on the **SPCR ACPI table** to obtain UART information.  
  Set the UEFI console to **serial**.
- ITS tests are available only for systems presenting firmware compliant to SBBR.
- Some PCIe and Exerciser tests depend on platform PCIe features. Please populate the required PAL APIs with platform details:
  - `pal_pcie_p2p_support` — whether peer-to-peer transactions are supported.
  - `pal_pcie_is_cache_present` — whether PCIe address translation cache is supported.
  - `pal_pcie_get_legacy_irq_map` — fill the system legacy IRQ map.
- Required Exerciser capabilities:
  - MSI-X interrupt generation.
  - Incoming Transaction Monitoring (order, type).
  - Initiating transactions from and to the Exerciser.
  - Ability to check BDF and register addresses observed for each configuration access along with access type.
- Linux DMA-related tests have **not** been verified


## License
BSA ACS is distributed under the **Apache v2.0 License**.

--------------

*Copyright (c) 2021-2025, Arm Limited and Contributors. All rights reserved.*
