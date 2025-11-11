## 📚 Table of Contents

* [Introduction](#introduction)
* [Repository Structure](#repository-structure)
* [Architecture Compliance Suites](#architecture-compliance-suites)
  * [BSA ACS](#bsa-architecture-compliance-suite)
  * [SBSA ACS](#sbsa-architecture-compliance-suite)
  * [SYS-MPAM ACS](#sys-mpam-architecture-compliance-suite)
  * [PC-BSA ACS](#pc-bsa-architecture-compliance-suite)
  * [DRTM ACS](#drtm-architecture-compliance-suite)
  * [MemTest ACS](#memtest-architecture-compliance-suite)
  * [PFDI ACS](#pfdi-architecture-compliance-suite)
* [Security Implications](#security-implications)
* [Limitations](#limitations)
* [Feedback and Support](#feedback-and-support)
* [License](#license)

[![SYSARCH-ACS Build Check](https://github.com/ARM-software/sysarch-acs/actions/workflows/sysarch_daily.yml/badge.svg)](https://github.com/ARM-software/sysarch-acs/actions/workflows/sysarch_daily.yml)

---

## 🏗️ Introduction

The **Arm sysarch-acs** repository is a collection of **Architecture Compliance Suites (ACS)** that verify compliance with the following Arm System Architecture specifications:

* **[BSA – Base System Architecture](https://developer.arm.com/documentation/den0094/latest/)**
* **[SBSA – Server Base System Architecture](https://developer.arm.com/documentation/den0029/latest/)**
* **[PC-BSA – Personal Computing Base System Architecture](https://developer.arm.com/documentation/den0151/latest/)**
* **[MPAM (Memory system resource partitioning and monitoring) system component](https://developer.arm.com/documentation/ihi0099/latest/)**
* **[DRTM – Dynamic Root of Trust for Measurement](https://developer.arm.com/documentation/den0113/latest/)**
* **[PFDI – Platform Fault Detection Interface](https://documentation-service.arm.com/static/68593b17961937560be90fbe)**

Additionally, it also includes ACS for ...
* **MemTest – Memory Model Consistency tests**

These suites enable silicon vendors, system integrators, and firmware developers to ensure platform compliance with Arm architectural standards across pre-silicon and silicon phases.

---

## 📁 Repository Structure

```
sysarch-acs/
├── apps/
│   ├── baremetal/
│   |   └── <acs_name>_main.c
│   ├── linux/
│   |   └── <acs_name>_app
│   ├── uefi/
│   |   └── <acs_name>_main.c
├── docs/
│   ├── bsa/
│   ├── sbsa/
│   ├── drtm/
│   ├── pfdi/
│   ├── mpam/
│   └── pc_bsa/
├── test_pool/
│   ├── <module>/
├── val/
├── pal/
│   └── baremetal/
│   └── uefi_acpi/
│   └── uefi_dt/
├── tools/
├── mem_test/
|── prebuilt_images/
│   ├── bsa/
│   ├── sbsa/
│   ├── drtm/
│   ├── pfdi/
│   ├── mpam/
│   └── pc_bsa/
```


* (`docs/<acs_name>`) : The documentation provides guides and reference materials to help partners onboard ACS
* (`prebuilt_images/<acs>`) : Prebuilt ACS **release** binaries location for each supported acs
* (`tools/scripts`) : Contains build scripts
* (`apps/`) : Includes the application code for each ACS, which serves as the main entry point to invoke test execution
* (`test_pool/`) : ACS test source files arranged in module basis
* (`val/`) : Validation Abstraction Layer providing a platform-independent interface for ACS tests.
* (`pal/`) : PAL abstraction for platform integration

---

## 🧱 Architecture Compliance Suites

### 🧩 BSA Architecture Compliance Suite

Validates platform compliance with Arm BSA specification.

#### 🧾 Latest Release

* **Version:** v1.1.1
* **Code Quality:** REL
* **Spec Reference:** [BSA 1.1](https://developer.arm.com/documentation/den0094/d/?lang=en)
* **Exerciser Support:** Required for complete coverage
* **Prebuilt Binaries:** [prebuilt_images/bsa/](prebuilt_images/BSA)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [BSA ACS README](docs/bsa/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 SBSA Architecture Compliance Suite

Validates platform compliance with Arm SBSA specification.

#### 🧾 Latest Release

* **Version:** v7.2.3
* **Code Quality:** REL
* **Spec Reference:** [SBSA 7.2](https://developer.arm.com/documentation/den0029/i/?lang=en)
* **Complementary Tests:** BSA ACS required for full coverage
* **Exerciser Support:** Required for complete coverage
* **Prebuilt Binaries:** [prebuilt_images/sbsa/](prebuilt_images/SBSA)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [SBSA ACS README](docs/sbsa/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 SYS-MPAM Architecture Compliance Suite

Validates platform compliance with Arm MPAM system component specification.

#### 🧾 Latest Release

* **Version:** v0.5.0
* **Code Quality:** REL
* **Spec Reference:** [MPAM system component 1.1](https://developer.arm.com/documentation/den0094/d/?lang=en)
* **Prebuilt Binaries:** [prebuilt_images/mpam/](https://github.com/ARM-software/bsa-acs/tree/main/prebuilt_images/MPAM)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [MPAM ACS README](docs/mpam/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 PC-BSA Architecture Compliance Suite

Validates platform compliance with Arm PCBSA specification.

#### 🧾 Latest Release

* **Version:** v0.8.0
* **Code Quality:** BETA
* **Spec Reference:** [PCBSA 1.0](https://developer.arm.com/documentation/den0151/latest/)
* **Prebuilt Binaries:** [prebuilt_images/pcbsa/](prebuilt_images/PCBSA)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [PC BSA ACS README](docs/pc_bsa/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 DRTM Architecture Compliance Suite

Validates platform compliance with Arm DRTM specification.

#### 🧾 Latest Release

* **Version:** v0.6.0
* **Code Quality:** ALPHA
* **Spec Reference:** [DRTM 1.1](https://developer.arm.com/documentation/den0113/c/?lang=en)
* **Prebuilt Binaries:** [prebuilt_images/drtm/](https://github.com/ARM-software/bsa-acs/tree/main/prebuilt_images/DRTM)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [DRTM ACS README](docs/drtm/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 MemTest (Memory Model Consistency Tests)

Evaluates the correctness and consistency of system memory model.

#### 🧾 Latest Release

* **Version:** v1.0.0
* **Code Quality:** EAC
* **Prebuilt Binaries:** [prebuilt_images/memtest/](https://github.com/ARM-software/bsa-acs/tree/main/prebuilt_images/MEM_TEST)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [MemTest ACS README](mem_test/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

### 🧩 PFDI Architecture Compliance Suite

Validates platform compliance with Arm PFDI specification.

#### 🧾 Latest Release

* **Version:** v0.8.0
* **Code Quality:** BETA
* **Spec Reference:** [PFDI](https://documentation-service.arm.com/static/68593b17961937560be90fbe)
* **Prebuilt Binaries:** [prebuilt_images/pfdi/](prebuilt_images/PFDI)
* **NOTE** The compliance suite is not a substitute for design verification

#### 📚 Reference for Build, Execution, and More
Refer to the [PFDI ACS README](docs/pfdi/README.md) for detailed build steps, execution procedures, additional information, and known limitations.

---

## 🔐 Security Implications

Running ACS requires elevated privileges.
⚠️ Only execute on **development** systems. Do **not** run on production systems without sanitization.

---

## 💬 Feedback and Support

* 📧 Email: [support-systemready-acs@arm.com](mailto:support-systemready-acs@arm.com)
* 🧩 Issues: [GitHub Tracker](../../issues)
* 🤝 Contributions: [GitHub Pull Requests](../../pulls)

---

## 📄 License

Distributed under [Apache v2.0 License](https://www.apache.org/licenses/LICENSE-2.0).
© 2025 Arm Limited and Contributors.

---
