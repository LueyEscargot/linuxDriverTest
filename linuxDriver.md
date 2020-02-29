# Linux Driver

<!-- TOC -->

- [1. Environment](#1-environment)
    - [1.1. Install Linux Kernel Source & Head files](#11-install-linux-kernel-source--head-files)
    - [1.2. Development](#12-development)
        - [1.2.1. Virtual Studio Code](#121-virtual-studio-code)
- [2. Misc](#2-misc)
    - [2.1. Signing Linux Driver Module](#21-signing-linux-driver-module)

<!-- /TOC -->

## 1. Environment

### 1.1. Install Linux Kernel Source & Head files

    ```sh
    apt install linux-source-5.3.0
    apt install linux-headers-5.3.0-40
    ```

### 1.2. Development

#### 1.2.1. Virtual Studio Code

- c_cpp_properties.json

  ```json
  {
    "configurations": [
      {
        "name": "Linux",
        "defines": [""],
        "compilerPath": "/usr/bin/gcc",
        "cStandard": "c11",
        "cppStandard": "c++17",
        "intelliSenseMode": "clang-x64",
        "includePath": [
          "${workspaceFolder}/**",
          "/usr/lib/gcc/x86_64-linux-gnu/7.4.0/include/",
          "/usr/src/linux-headers-5.3.0-40/arch/x86/include",
          "/usr/src/linux-headers-5.3.0-40/include",
          "/usr/src/linux-headers-5.3.0-40/include/uapi",
          "/usr/src/linux-headers-5.3.0-40-generic/arch/x86/include/generated",
          "/usr/src/linux-headers-5.3.0-40-generic/include"
        ]
      }
    ],
    "version": 4
  }
  ```

## 2. Misc

### 2.1. Signing Linux Driver Module

Ref:

- Linux 内核模块 ([https://jin-yang.github.io/post/kernel-modules.html](https://jin-yang.github.io/post/kernel-modules.html))
- Linux 内核模块签名 ([https://ihexon.github.io/develop/Signed-kernel-module-support/](https://ihexon.github.io/develop/Signed-kernel-module-support/))
- 从 0 开始学 Linux 驱动开发(一) ([https://paper.seebug.org/779/](https://paper.seebug.org/779/))

Step:

- create config file

  ```sh
  cat << EOF > configuration_file.config
  [ req ]
  default_bits = 8192
  distinguished_name = req_distinguished_section
  prompt = no
  string_mask = utf8only
  x509_extensions = x509_extensions_section

  [ req_distinguished_section ]
  O = AiryBit
  CN = AiryBit Linux Driver Certificate Authority
  emailAddress = admin@airybit.com

  [ x509_extensions_section ]
  basicConstraints=critical,CA:FALSE
  keyUsage=digitalSignature
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid
  EOF
  ```

- generate key file

  ```sh
  openssl req -x509 -new -nodes -utf8 -sha256 -days 36500 \
  -batch -config configuration_file.config -outform DER \
  -out public_key.der -keyout private_key.priv
  ```

- mok import

  ```sh
  sudo mokutil --import public_key.der
  # input password

  # reboot system
  reboot
  ```

- check key ring

  ```sh
  # install keyutils
  sudo apt install keyutils

  # list key ring
  keyctl list %:.system_keyring | grep "Organization"
  cat /proc/keys

  ## or ##
  dmesg | grep 'EFI: Loaded cert'
  ```

- signing module

  ```sh
  # For Debian / Ubuntu
  /usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der helloWorld.ko

  # For RedHat / CentOS
  /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der hello.ko
  ```

- install module

  ```sh
  insmod helloWorld.ko
  ```
