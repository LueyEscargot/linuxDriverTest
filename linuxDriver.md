# Linux Driver

<!-- TOC -->

- [1. Signing Linux Driver Module](#1-signing-linux-driver-module)
    - [1.1. Step](#11-step)

<!-- /TOC -->

## 1. Signing Linux Driver Module

Ref:

- Linux 内核模块 ([https://jin-yang.github.io/post/kernel-modules.html](https://jin-yang.github.io/post/kernel-modules.html))
- Linux 内核模块签名 ([https://ihexon.github.io/develop/Signed-kernel-module-support/](https://ihexon.github.io/develop/Signed-kernel-module-support/))
- 从 0 开始学 Linux 驱动开发(一) ([https://paper.seebug.org/779/](https://paper.seebug.org/779/))

### 1.1. Step

- create config file

    ```sh
    cat << EOF > configuration_file.config
    [ req ]
    default_bits = 4096
    distinguished_name = req_distinguished_name
    prompt = no
    string_mask = utf8only
    x509_extensions = myexts

    [ req_distinguished_name ]
    O = Organization
    CN = Organization signing key
    emailAddress = E-mail address

    [ myexts ]
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
    /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der helloWorld.ko
    ```

- install module

    ```sh
    insmod helloWorld.ko
    ```
