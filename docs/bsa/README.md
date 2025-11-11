# Base System Architecture - Architecture Compliance Suite

[![BSA-ACS Build Check](https://github.com/ARM-software/bsa-acs/actions/workflows/bsa-acs_build_check.yml/badge.svg)](https://github.com/ARM-software/bsa-acs/actions/workflows/bsa-acs_build_check.yml)

## Table of Contents

- [Base System Architecture](#base-system-architecture)
- [BSA - Architecture Compliance Suite](#bsa---architecture-compliance-suite)
- [Release details](#release-details)
- [BSA UEFI tests build steps - UEFI Shell application](#acs-build-steps---uefi-shell-application)
  - [Steps for platform using ACPI table](#to-start-the-acs-build-for-platform-using-acpi-table-perform-the-following-steps)
  - [Steps for platform using Device tree](#to-start-the-acs-build-for-platform-using-device-tree-perform-the-following-steps)
- [BSA Linux tests build steps - Linux application](#acs-build-steps---linux-application)
  - [Building the Kernel Module, App (Script)](#11-building-the-kernel-module-app-script)
  - [Building the Kernel Module, App (Manual)](#12-building-the-kernel-module-app-manual)
-[BSA UEFI tests Run Steps ](#2-test-suite-execution)
    - [2.1 Silicon](#21-silicon)
    - [2.2 Emulation environment with secondary storage](#22-emulation-environment-with-secondary-storage)
    - [2.3 Emulation environment without secondary storage](#23-emulation-environment-without-secondary-storage)
  - [BSA Linux test run steps](#2-steps-for-running-bsa-tests-in-linux)
  - [Build Script arguments](#build-script-arguments)
  - [Limitations](#limitations-1)
- [ACS build steps - Bare-metal abstraction](#acs-build-steps---bare-metal-abstraction)
- [Security implication](#security-implication)
- [Limitations](#limitations)
  - [BSA ACS version mapping](#bsa-acs-version-mapping)
- [License](#license)
- [Feedback, contributions, and support](#feedback-contributions-and-support)


## Base System Architecture
**Base System Architecture (BSA)** describes a 64-bit Arm-based hardware system architecture relied upon by operating systems, hypervisors, and firmware. It addresses PE features and key aspects of system architecture.

For more information, download the [BSA specification](https://developer.arm.com/documentation/den0094/d/?lang=en).

## BSA - Architecture Compliance Suite

The BSA **Architecture Compliance Suite (ACS)** is a collection of self-checking, portable C-based tests.
This suite provides examples of the invariant behaviors defined in the [BSA specification](https://developer.arm.com/documentation/den0094/d/?lang=en), enabling verification that these behaviors have been implemented and interpreted correctly.

Most tests run from the UEFI (Unified Extensible Firmware Interface) Shell via the BSA UEFI shell application.
A subset runs from Linux via a BSA ACS user-space application and its kernel module.
Tests can also run in a bare-metal environment. Initialization of the bare-metal environment is platform-specific and out of scope for this document.

## Release details
- **Code quality:** v1.1.0  
- **Spec version:** Tests target BSA specification v1.1.  
- **Execution levels:** Pre-Silicon and Silicon.  
- **Exerciser dependency:** For complete coverage, an Exerciser is required to run Exerciser tests during verification at Pre-Silicon level.  
- **Scope:** The compliance suite is **not** a substitute for design verification.  
- **Access to logs:** Arm licensees can contact Arm through their partner managers.  
- **Unimplemented rules:** See the [Test Scenario Document](docs/arm_bsa_architecture_compliance_test_scenario.pdf).

### BSA ACS version mapping
--------------------------------------------------------------------------------------------
|    BSA Spec Version   |   BSA ACS Version   |      BSA Tag ID     |    Pre-Si Support    |
|-----------------------|:-------------------:|:-------------------:|:--------------------:|
|       BSA v1.1        |        v1.1.0       |   v25.03_REL1.1.0   |       Yes            |
|       BSA v1.1        |        v1.0.9       |   v24.11_REL1.0.9   |       Yes            |
|       BSA v1.0(c)     |        v1.0.8       |   v24.03_REL1.0.8   |       Yes            |
|       BSA v1.0(c)     |        v1.0.7       |   v23.12_REL1.0.7   |       Yes            |
|       BSA v1.0(c)     |        v1.0.6       |v23.11_BootFramework |       Yes            |
|       BSA v1.0(c)     |        v1.0.6       |   v23.09_REL1.0.6   |       Yes            |
|       BSA v1.0(c)     |        v1.0.5       |   v23.07_REL1.0.5   |       Yes            |
|       BSA v1.0(c)     |        v1.0.4       |   v23.03_REL1.0.4   |       Yes            |
|       BSA v1.0        |        v1.0.3       |   v23.01_REL1.0.3   |       No             |
|       BSA v1.0        |        v1.0.2       |   v22.10_REL1.0.2   |       No             |
|       BSA v1.0        |        v1.0.1       |   v22.06_REL1.0.1   |       No             |
|       BSA v1.0        |        v1.0         |   v21.09_REL1.0     |       No             |
--------------------------------------------------------------------------------------------

### GitHub branch
- To pick up the release version of the code, check out the corresponding **tag** from the **main** branch.
- To get the latest code with bug fixes and new features, use the **main** branch.

### Prebuilt images
Prebuilt images for each release are available in the `prebuilt_images` folder of the main branch.  
You may use these directly or build your own images using the steps below. If you use a prebuilt image, see **Test suite execution** for runtime instructions.

### Additional reading
- [Arm BSA Test Scenario Document](docs/arm_bsa_architecture_compliance_test_scenario.pdf) — algorithms for implementable rules and notes on unimplemented rules.  
- [Arm BSA Test Checklist](docs/arm_bsa_testcase_checklist.rst) — test categories (UEFI, Linux, Bare-metal) and applicable systems (IR, ES, SR, Pre-Silicon).  
- [Arm BSA Validation Methodology](docs/arm_bsa_architecture_compliance_validation_methodology.pdf).  
- [Arm BSA ACS User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf).  
- **Bare-metal support**  
  - [Arm BSA ACS Bare-metal User Guide](docs/arm_bsa_architecture_compliance_bare-metal_user_guide.pdf)  
  - [Bare-metal Code](pal/baremetal/)  

> **Note:** The Bare-metal PCIe enumeration code provided with BSA ACS must be used and **must not be replaced**. It is essential for accurate analysis of test results.

### Running Exerciser tests for complete coverage
The **Exerciser** is a client device wrapped by a PCIe Endpoint, created to satisfy BSA requirements for PCIe capability validation. Running Exerciser tests increases platform coverage.

> **UEFI-based platforms:** Exerciser PAL APIs must be implemented. See [Exerciser.md](docs/PCIe_Exerciser/Exerciser.md) and [Exerciser_API_porting_guide.md](docs/PCIe_Exerciser/Exerciser_API_porting_guide.md).

## ACS build steps - UEFI Shell application

### 1. Building from source
Before you start the ACS build, ensure that the following requirements are met:

- A mainstream Linux distribution on x86 or AArch64.
- Clone the [EDK2 tree](https://github.com/tianocore/edk2). Recommended commit: `836942fbadb629050b866a8052e6af755bcdf623`.
- Clone the [EDK2 port of libc](https://github.com/tianocore/edk2-libc) into local `<edk2_path>`.
- Install **GCC-ARM 13.2** [toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
- Install prerequisite packages to build EDK2.  
  *Note: Package details are beyond the scope of this document.*

#### 1.1 Target Platform

##### To start the ACS build for platform using ACPI table, perform the following steps:
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

##### To start the ACS build for platform using Device tree, perform the following steps:
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

6. **IR systems:** The ACS EFI application runs on top of UEFI Shell which runs on **U-Boot as EFI payload**.  
   Apply the following change in `edk2/MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.c` **before compiling for IR**:
   ```diff
   -Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
   -ASSERT_EFI_ERROR (Status);
   +//Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &gHiiConfigRouting);
   +//ASSERT_EFI_ERROR (Status);
   ```

#### 1.2 Build environment

##### If the build environment is Linux, perform the following steps:
1. `export GCC49_AARCH64_PREFIX=<GCC 13.2 toolchain path>/bin/aarch64-linux-gnu-` (on x86 hosts)  
   For an AArch64 host build, this should point to `/usr/bin/`.
2. `export PACKAGES_PATH=<path to edk2-libc>`  
3. `source edksetup.sh`  
4. `make -C BaseTools/Source/C`  
5. `source ShellPkg/Application/bsa-acs/tools/scripts/acsbuild.sh`

#### 1.3 Build output
The EFI executable is generated at:  
`<edk2_path>/Build/Shell/DEBUG_GCC49/AARCH64/Bsa.efi`

### 2. Test suite execution

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

#### 2.1 Silicon
On a system with a functional USB port:

**For IR Systems**
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

#### 2.2 Emulation environment with secondary storage
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

#### 2.3 Emulation environment without secondary storage
1. Add the path to `Bsa.efi` in the UEFI FD file.  
2. Build a UEFI image including **UEFI Shell**.  
3. Boot to UEFI Shell.  
4. Run `Bsa.efi` to start tests. (See the [User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf) for parameters.)  
5. Capture UART logs for analysis and certification.

## ACS build steps - Linux application

Certain Peripheral, PCIe, and Memory Map tests require a Linux OS. This section covers building and executing these tests from the Linux application.

### 1. Build steps and environment setup
Patches for the kernel tree and the Linux PAL are hosted separately in the [linux-acs](https://git.gitlab.arm.com/linux-arm/linux-acs.git) repository.

#### Prerequisites
*Skip this if using the [Linux Build Script](https://gitlab.arm.com/linux-arm/linux-acs/-/blob/master/acs-drv/files/build.sh?ref_type=heads).*
- Supported kernel versions: **5.11, 5.13, 5.15, 6.0, 6.4, 6.7, 6.8**  
- Toolchain: **GCC-ARM 13.2**  
- AArch64 Linux kernel build environment.  
  > **Note:** Linux **6.8** is the recommended version.

#### 1.1 Building the Kernel Module, App (Script)
The `build.sh` script supports native builds and cross-compilation.
- **Native builds:** run directly on the target machine.
- **Cross-compilation:** pass the Linux version and GCC tool version as script arguments.

##### Linux Build Steps (Script)
```sh
wget https://gitlab.arm.com/linux-arm/linux-acs/-/raw/master/acs-drv/files/build.sh
chmod +x build.sh
source build.sh
```

##### Build Output
An output folder `build/` is created containing:
- `bsa_acs.ko`
- `bsa_app`

#### 1.2 Building the Kernel Module, App (Manual)
Manual build steps for the BSA kernel module and application:

##### Build steps for BSA kernel module
```sh
git clone https://git.gitlab.arm.com/linux-arm/linux-acs.git linux-acs
git clone https://github.com/ARM-software/bsa-acs.git bsa-acs
cd <local_dir>/linux-acs/files
export CROSS_COMPILE=<ARM64 toolchain path>/bin/aarch64-linux-gnu-
export KERNEL_SRC=/lib/modules/$(uname -r)/build
./bsa_setup.sh <local_dir/bsa-acs>
./linux_bsa_acs.sh
```

**Note:**
- If `/lib/modules/$(uname -r)/build` does not exist on the native system, install headers:
  ```sh
  sudo apt-get install linux-headers-$(uname -r)
  ```
- For cross-compilation, set `KERNEL_SRC` to the kernel build output directory for the target architecture.

Successful completion generates `bsa_acs.ko`.

##### BSA Linux application build
```sh
cd <bsa-acs path>/linux_app/bsa-acs-app
export CROSS_COMPILE=<ARM64 toolchain path>/bin/aarch64-linux-gnu-
make
```

Successful completion generates the executable `bsa`.

### 2. Steps for Running BSA Tests in Linux

#### 2.1 Loading the kernel module
```sh
sudo insmod bsa_acs.ko
```

#### 2.2 Running BSA ACS
```sh
./bsa_app
# or
./bsa
```
- For application parameters, see the [User Guide](docs/arm_bsa_architecture_compliance_user_guide.pdf).

#### 2.3 BSA Linux Test Log View
```sh
sudo dmesg | tail -500   # print last 500 kernel logs
```

#### 2.4 Remove the BSA module
```sh
sudo rmmod bsa_acs
```

### Build Script arguments
The following arguments can be used with `build.sh`:

- `-v` or `--version` — Linux kernel version for cross-compilation. Default: **6.8**.  
- `--GCC_TOOLS` — GCC toolchain version for cross-compilation. Default: **13.2.rel1**.  
- `--help` — Displays environment info, defaults, usage, and notes.  
- `--clean` — Removes the `build/` output folder (modules and apps).  
- `--clean_all` — Removes all downloaded repositories and build artifacts, including the output directory.

### Limitations
> **Note:** DMA-related tests have **not** been verified.

For cross-compilation platforms, ensure the Linux source version matches the target device kernel version.

**Example:**
- Linux source version: `5.15`  
- Target AArch64 machine kernel version: `5.15.0-139-generic`

- If versions do not match exactly, the module may fail to load due to an **invalid module format**.  
- ✅ If both versions are identical (e.g., both 5.15), the build will work correctly — similar to SystemReady images.

## ACS build steps - Bare-metal abstraction
The bare-metal build environment is platform-specific.
As a baseline, steps to integrate and run the bare-metal tests from UEFI Shell are provided in [README.md](pal/baremetal/target/RDN2/README.md).
For details on generating binaries for bare-metal environments, refer to [README.md](pal/baremetal/README.md).


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

## Security implication
The Arm SystemReady ACS test suite may run at high privilege levels. An attacker could potentially use tests to elevate privilege and access platform security assets.  
To prevent leakage of secure information, Arm **strongly recommends** running ACS **only on development platforms**. If run on production systems, ensure the system is **scrubbed after execution**.

## License
BSA ACS is distributed under the **Apache v2.0 License**.

## Feedback, contributions, and support
- For feedback, use the GitHub Issue Tracker associated with this repository.
- For support, email **support-systemready-acs@arm.com** with details.
- Arm licensees may contact Arm directly through their partner managers.
- Arm welcomes code contributions through GitHub pull requests. See GitHub documentation for guidance.

--------------

*Copyright (c) 2021-2025, Arm Limited and Contributors. All rights reserved.*
