![version](https://img.shields.io/badge/version-17%2B-3E8B93)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-common-crypto)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-common-crypto/total)

# 4d-plugin-common-crypto
Collection of common hash algorithms based on native functions and some help from [OpenSSL](https://www.openssl.org).

### News

- library update ``1.1.1c`` (``1.1.1j`` on Mac)

## Installation Instructions
1. Extract either the **Common Crypto.DMG** or the **Comon Crypto.zip** archives
2. Add the **Common Crypto.bundle** to your database's **Plugins** folder.

See also [discuss.4d.com](https://discuss.4d.com/t/crash-on-using-aes128-and-aes256-in-plugin-commoncrypto/15770/2)

### Syntax

[miyako.github.io](https://miyako.github.io/2019/08/16/4d-plugin-common-crypto.html)

### Examples

* HASH

```4d
$source:="Hello World!"
$key:="my_secret_key"

CONVERT FROM TEXT($source;"utf-8";$sourceData)
CONVERT FROM TEXT($key;"utf-8";$keyData)

ASSERT(MD5 ($sourceData;Crypto BASE64)="7Qdih1MuhjZehB6Sv8UNjA==")
ASSERT(SHA1 ($sourceData;Crypto BASE64)="Lve95gjOVATpfV8EL5X4nxwjKHE=")
ASSERT(SHA256 ($sourceData;Crypto BASE64)="f4OxZX/x/FO5LcGBSKHWXfwtSx+j1ncoSt3SABJtkGk=")
ASSERT(SHA384 ($sourceData;Crypto BASE64)="v9dsDrvQBv7lg0EFR8GIewKSvnbVgtlsJC0qeScj4/1v0GH51c/RO4+WE1jmrbpK")
ASSERT(SHA512 ($sourceData;Crypto BASE64)="hhhE1nBOhXP+w02WfiC8/vPUJM9IvgTm3AjyvVjHKXQzcQFerYkcw88cnTS0kmS1EHUbH/nlN5N7xGtdb/TsyA==")

ASSERT(MD5 ($sourceData;Crypto HEX)="ed076287532e86365e841e92bfc50d8c")
ASSERT(SHA1 ($sourceData;Crypto HEX)="2ef7bde608ce5404e97d5f042f95f89f1c232871")
ASSERT(SHA256 ($sourceData;Crypto HEX)="7f83b1657ff1fc53b92dc18148a1d65dfc2d4b1fa3d677284addd200126d9069")
ASSERT(SHA384 ($sourceData;Crypto HEX)="bfd76c0ebbd006fee583410547c1887b0292be76d582d96c242d2a792723e3fd6fd061f9d5cfd13b8f961358e6adba4a")
ASSERT(SHA512 ($sourceData;Crypto HEX)="861844d6704e8573fec34d967e20bcfef3d424cf48be04e6dc08f2bd58c729743371015ead891cc3cf1c9d34b49264b510751b1ff9e537937bc46b5d6ff4ecc8")


ASSERT(HMACMD5 ($keyData;$sourceData;Crypto BASE64)="AYH2AOvu2Cbt3Ef/hR9k7A==")
ASSERT(HMACSHA1 ($keyData;$sourceData;Crypto BASE64)="BwpO356r2gx9SYj3UFVvc/BfdVA=")
ASSERT(HMACSHA256 ($keyData;$sourceData;Crypto BASE64)="5z1prOAxmam9lz5nlHgudpxIhksTY8pClCivq7RUytQ=")
ASSERT(HMACSHA384 ($keyData;$sourceData;Crypto BASE64)="UDZdkjSVyMpFJQu9dmeAlKg+LW97o6yq1pfBj3QZJM4YMmgZxkoUVVUzyMDcP3DT")
ASSERT(HMACSHA512 ($keyData;$sourceData;Crypto BASE64)="x38L+mMFPEM+QqGrPMOzx2l7rTlmNePxPdaYllFiLQm+G9K2Bf/9oQcci861HAF4NTNqsPZS8gPEiQ0KoznMlA==")

ASSERT(HMACMD5 ($keyData;$sourceData;Crypto HEX)="0181f600ebeed826eddc47ff851f64ec")
ASSERT(HMACSHA1 ($keyData;$sourceData;Crypto HEX)="070a4edf9eabda0c7d4988f750556f73f05f7550")
ASSERT(HMACSHA256 ($keyData;$sourceData;Crypto HEX)="e73d69ace03199a9bd973e6794782e769c48864b1363ca429428afabb454cad4")
ASSERT(HMACSHA384 ($keyData;$sourceData;Crypto HEX)="50365d923495c8ca45250bbd76678094a83e2d6f7ba3acaad697c18f741924ce18326819c64a14555533c8c0dc3f70d3")
ASSERT(HMACSHA512 ($keyData;$sourceData;Crypto HEX)="c77f0bfa63053c433e42a1ab3cc3b3c7697bad396635e3f13dd6989651622d09be1bd2b605fffda1071c8bceb51c017835336ab0f652f203c4890d0aa339cc94")
```

* RSA

```4d
$filePath:=Get 4D folder(Current resources folder)+"95d6465982187ad13e7c5badb657f80dc3e7f925-privatekey.p12"
DOCUMENT TO BLOB($filePath;$p12Data)

  //effectively the same as openssl pkcs12 -in *.p12 -out *.pem -nodes
$pemText:=PEM From P12 ($p12Data;$pemData;"notasecret")

C_BLOB($someDataToSign)
SET BLOB SIZE($someDataToSign;512)

ASSERT(RSASHA256 ($someDataToSign;$pemData;Crypto BASE64)="SFoEVXo/EqSagiapclf0m2J/d8wQPSq3So/G0EQeGZlpTnsBLQK1LGh9kqySf4yjmd1qdtrtNvmPkBkhNBHNNS9TfBD3OGipXFFE3aG2TSDoicz7yw5bASXD2k0ystqxWPJVk2TpucRBP2Z8dreZviKNqB+ljkWN/zy1vD1FlI8=")
```

* AES
 
```4d
CONVERT FROM TEXT("4D4D";"utf-8";$password)
CONVERT FROM TEXT("data"*4;"utf-8";$data)

$encrypted:=AES256 ($data;$password;Crypto Encrypt;Crypto AES ECB;Crypto BASE64)
BASE64 DECODE($encrypted;$data)

$decrypted:=AES256 ($data;$password;Crypto Decrypt;Crypto AES ECB;Crypto BASE64)
BASE64 DECODE($decrypted;$data)
$decrypted:=Convert to text($data;"utf-8")
```

* AES with IV

```4d
CONVERT FROM TEXT("data"*4;"utf-8";$data)

  //define key and iv instead of a password
C_BLOB($key;$iv)
SET BLOB SIZE($key;32)
SET BLOB SIZE($iv;16)

$iv{0x0000}:=0x0000
$iv{0x0001}:=0x0001
$iv{0x0002}:=0x0002
$iv{0x0003}:=0x0003
$iv{0x0004}:=0x0004
$iv{0x0005}:=0x0005
$iv{0x0006}:=0x0006
$iv{0x0007}:=0x0007
$iv{0x0008}:=0x0008
$iv{0x0009}:=0x0009
$iv{0x000A}:=0x000A
$iv{0x000B}:=0x000B
$iv{0x000C}:=0x000C
$iv{0x000D}:=0x000D
$iv{0x000E}:=0x000E
$iv{0x000F}:=0x000F

$key{0x0000}:=0x002B
$key{0x0001}:=0x007E
$key{0x0002}:=0x0015
$key{0x0003}:=0x0016
$key{0x0004}:=0x0028
$key{0x0005}:=0x00AE
$key{0x0006}:=0x00D2
$key{0x0007}:=0x00A6
$key{0x0008}:=0x00AB
$key{0x0009}:=0x00F7
$key{0x000A}:=0x0015
$key{0x000B}:=0x0088
$key{0x000C}:=0x0009
$key{0x000D}:=0x00CF
$key{0x000E}:=0x004F
$key{0x000F}:=0x003C

$encrypted:=AES256 ($data;"";Crypto Encrypt;Crypto AES CTR;Crypto BASE64;Crypto AES No padding;$key;$iv)
BASE64 DECODE($encrypted;$data)

$decrypted:=AES256 ($data;"";Crypto Decrypt;Crypto AES CTR;Crypto BASE64;Crypto AES No padding;$key;$iv)
BASE64 DECODE($decrypted;$data)
$decrypted:=Convert to text($data;"utf-8")

ASSERT($decrypted=("data"*4))
```

* RIPEMD160

```4d
$source:="Hello World!"
$key:="my_secret_key"

CONVERT FROM TEXT($source;"utf-8";$sourceData)

ASSERT(RIPEMD160 ($sourceData;Crypto BASE64)="hHbuRjG5swrCdUsO4MR+Fh0/ckw=")
```

* Verify

```4d
CONVERT FROM TEXT("abcde";"utf-8";$data)

DOCUMENT TO BLOB(System folder(Desktop)+"prvkey.pem";$prvKey)
$data64:=RSASHA1 ($data;$prvKey;Crypto BASE64)

DOCUMENT TO BLOB(System folder(Desktop)+"pubkey.pem";$pubKey)
$OK:=RSAVERIFYSHA1 ($data;$pubKey;$data64;Crypto BASE64)
```
