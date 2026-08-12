![version](https://img.shields.io/badge/version-17%2B-3E8B93)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-common-crypto)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-common-crypto/total)

# 4d-plugin-common-crypto

Common Crypto wraps OpenSSL to give 4D methods direct access to hashing (MD5/SHA-1/SHA-2/SHA-3/SHAKE/RIPEMD-160), HMAC, PBKDF2 key derivation, AES encryption/decryption, RSA signing and verification, and PEM/DER/PKCS#12 certificate handling. Binary input/output is always a `Blob`; results are returned as `Text`, pre-encoded as hex, Base64, or Base32 depending on an encoding parameter you pass. All cryptographic work happens through OpenSSL's `EVP_*` API underneath — no platform-native crypto (CommonCrypto/BCrypt) is used, so behavior is identical on both supported platforms.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [PEM From P12](#pem-from-p12) | Text | Extract a private key and certificate chain from a PKCS#12 (`.p12`/`.pfx`) file as PEM |
| [Get_timestamp](#get_timestamp) | Text | Current UTC time as `YYYY-MM-DDTHH:MM:SSZ` |
| [Get_timestring](#get_timestring) | Text | Current UTC time as an RFC-1123-style string |
| [Get_unixtime](#get_unixtime) | Text | Current Unix time (seconds since epoch) as text |
| [MD5](#md5) | Text | MD5 digest |
| [RIPEMD160](#ripemd160) | Text | RIPEMD-160 digest |
| [SHA1](#sha1) | Text | SHA-1 digest |
| [SHA224](#sha224) | Text | SHA-224 digest |
| [SHA256](#sha256) | Text | SHA-256 digest |
| [SHA384](#sha384) | Text | SHA-384 digest |
| [SHA512](#sha512) | Text | SHA-512 digest |
| [SHA512_224](#sha512_224) | Text | SHA-512/224 digest |
| [SHA512_256](#sha512_256) | Text | SHA-512/256 digest |
| [RSASHA1](#rsasha1) | Text | Sign a SHA-1 digest with an RSA private key |
| [RSASHA256](#rsasha256) | Text | Sign a SHA-256 digest with an RSA private key |
| [RSAVERIFYSHA1](#rsaverifysha1) | Longint | Verify an RSA/SHA-1 signature |
| [RSAVERIFYSHA256](#rsaverifysha256) | Longint | Verify an RSA/SHA-256 signature |
| [AES128](#aes128) | Text | AES-128 encrypt/decrypt |
| [AES192](#aes192) | Text | AES-192 encrypt/decrypt |
| [AES256](#aes256) | Text | AES-256 encrypt/decrypt |
| [SHA3_224](#sha3_224) | Text | SHA3-224 digest |
| [SHA3_256](#sha3_256) | Text | SHA3-256 digest |
| [SHA3_384](#sha3_384) | Text | SHA3-384 digest |
| [SHA3_512](#sha3_512) | Text | SHA3-512 digest |
| [SHAKE128](#shake128) | Text | SHAKE128 extendable-output digest |
| [SHAKE256](#shake256) | Text | SHAKE256 extendable-output digest |
| [HMACMD5](#hmacmd5) | Text | HMAC-MD5 |
| [HMACSHA1](#hmacsha1) | Text | HMAC-SHA1 |
| [HMACSHA224](#hmacsha224) | Text | HMAC-SHA224 |
| [HMACSHA256](#hmacsha256) | Text | HMAC-SHA256 |
| [HMACSHA384](#hmacsha384) | Text | HMAC-SHA384 |
| [HMACSHA512](#hmacsha512) | Text | HMAC-SHA512 |
| [HMACSHA512_224](#hmacsha512_224) | Text | HMAC-SHA512/224 |
| [HMACSHA512_256](#hmacsha512_256) | Text | HMAC-SHA512/256 |
| [HMACSHA3_224](#hmacsha3_224) | Text | HMAC-SHA3-224 |
| [HMACSHA3_256](#hmacsha3_256) | Text | HMAC-SHA3-256 |
| [HMACSHA3_384](#hmacsha3_384) | Text | HMAC-SHA3-384 |
| [HMACSHA3_512](#hmacsha3_512) | Text | HMAC-SHA3-512 |
| [PBKDF2_HMAC_SHA1](#pbkdf2_hmac_sha1) | Text | Derive a key with PBKDF2-HMAC-SHA1 |
| [PEMInfo](#peminfo) | Text | Human-readable certificate dump from a PEM blob |
| [DERToPEM](#dertopem) | Text | Convert a DER certificate to PEM, plus a text dump |

**Platforms:** macOS, Windows.

---

## Requirements & platform notes

- **No platform divergence.** Every command routes through OpenSSL's cross-platform `EVP_*`/`PEM_*`/`PKCS12_*` API; there is no `#if VERSIONMAC`/`#if VERSIONWIN` branch anywhere in the crypto code itself. The only platform-specific code in the plugin at all is the `TZ` environment variable set inside `Get_timestamp`/`Get_timestring` (`setenv` on macOS, `_wputenv_s` on Windows) — functionally equivalent on both.
- **Output encoding parameter (shared convention).** Every command that returns a `Text` digest, signature, or ciphertext takes a trailing `encoding` `Longint` parameter with the same four values throughout the plugin:

  | Value | Encoding |
  |---|---|
  | *(anything other than 1, 2, or 3)* | Hexadecimal (lowercase) |
  | `1` | Base64 |
  | `2` | A second Base64 variant (the plugin's underlying Base64 encoder is called with a second flag set; exact difference from `1` — e.g. URL-safe vs. unpadded — isn't confirmed from the header available for this review) |
  | `3` | Base32 |

  Any command's parameter table below that says "see the encoding table above" uses exactly this mapping.
- **Blob in, Blob out.** All binary input (data to hash, keys, IVs, certificates, PKCS#12 files) is passed as `Blob`. If you're starting from `Text`, convert it first with `CONVERT FROM TEXT`, exactly as every provided test method does.
- **`SHAKE128`/`SHAKE256` — known output-encoding limitation.** These two commands take a length parameter and an encoding parameter like every other hash command, but the encoding parameter is currently ignored — output is always hexadecimal regardless of what you pass as the third parameter, *unless* the length value you request happens to numerically coincide with `1`, `2`, or `3`, in which case you get that encoding by coincidence, not by request. This is a real bug in the current build, not a design choice — see [SHAKE128](#shake128)/[SHAKE256](#shake256) below for the mechanism. Treat output from these two commands as always-hex until this is fixed upstream.
- **`SHAKE256` — length rounding differs slightly from `SHAKE128`.** For a requested length that isn't an exact multiple of 4, `SHAKE256` currently rounds its output up by more bytes than `SHAKE128`'s equivalent rounding does for the same case. Both commands' rounding is otherwise identical for lengths that *are* multiples of 4 (the case exercised by this plugin's own test file). If you need an exact byte count, request a length that's a multiple of 4.
- **`SHAKE128`/`SHAKE256` length parameter is now bounded (as of this review).** Previously, a negative length value could crash the command (a null-pointer write) and an excessively large value could force a multi-hundred-megabyte allocation per call with no ceiling. As part of this review, negative values are now clamped to zero and the resulting output length is capped at 65,536 bytes before any allocation happens. **This bound is new** — it applies to the corrected source in this review, not necessarily to whatever binary you currently have installed.
- **AES-GCM and AES-CCM modes (`mode` = 7 or 8) do not produce a usable authentication tag.** `AES128`/`AES192`/`AES256` accept GCM and CCM as cipher modes, but the plugin never retrieves the tag on encrypt or supplies it on decrypt. Encrypting with GCM/CCM silently returns ciphertext with no tag attached — it will not authenticate against a standard GCM/CCM consumer, and there is no error or warning indicating this. **Avoid GCM/CCM with this plugin until this is addressed; use CBC, CFB, OFB, or CTR instead.**
- **Short AES keys/IVs are silently zero-padded, not rejected.** If you supply an explicit key (`$7`) or IV (`$8`) shorter than the cipher requires, the remaining bytes are filled with zeros rather than the command failing — e.g. an 8-byte key for AES-128 (which needs 16) becomes an 8-byte key plus 8 zero bytes. This weakens the effective key without any indication. Always supply keys/IVs at their exact required length (16/24/32 bytes for AES-128/192/256 keys; 16 bytes for most IVs).
- **`AES192` has no XTS mode.** `AES128` and `AES256` both accept mode `9` for XTS; `AES192` does not implement XTS in OpenSSL and passing `9` for `AES192`'s mode parameter silently falls back to ECB rather than returning an error.
- **Several `PBKDF2_HMAC_*` variants exist in the plugin's source but are not currently callable.** The binary only exposes `PBKDF2_HMAC_SHA1`. `PBKDF2_HMAC_MD5`, `_SHA224`, `_SHA256`, `_SHA384`, `_SHA512`, `_SHA512_224`, `_SHA512_256`, and four SHA-3 variants are implemented internally but are not wired into the plugin's command table, so they don't appear as 4D commands in this build. If you need one of them, that's a request for the plugin author, not something you can call today.
- **Command names below, beyond the ones confirmed in the plugin's own test methods, are the plugin's internal function names** (`SHA256`, `HMACSHA256`, etc.) — this matches the actual 4D command name for every case this review could confirm against a real test file, *except* `PEM_From_P12`, whose confirmed real command name is **`PEM From P12`** (with spaces). No `manifest.json` was available for this review, so if a name below other than `PEM From P12` doesn't match what you see in your Method list, trust your installed plugin's method list over this document and let the maintainer know.

---

## PEM From P12

**PEM From P12 ( p12Data ; pemData ; password ; options ) → Text**

### Syntax

```4d
result := PEM From P12 ( p12Data ; pemData ; password ; options )
```

| Parameter | Type | Description |
|---|---|---|
| `p12Data` | Blob | The raw contents of a `.p12`/`.pfx` file. |
| `pemData` | Blob | *(by reference)* Receives the PEM-encoded output (private key, and optionally the certificate/chain) as a blob. |
| `password` | Text | The PKCS#12 file's password. Also used as the PEM private-key encryption passphrase in the output. |
| `options` | Longint | Bitmask. Bit 1 (`1`): include the full certificate chain (only has effect if the P12 file actually contains chain certificates). Bit 2 (`2`): also include a human-readable `X509_print` dump of each certificate's details in the output. |
| Result | Text | The same PEM content written into `pemData`, returned as text. Empty if the P12 blob is invalid or the password is wrong. |

### Description

Parses the PKCS#12 blob with the given password, PEM-encodes the private key (re-encrypted with the same password) and, depending on `options`, the certificate and chain, and writes the result to both the `pemData` output parameter and the function result. If the blob isn't a valid PKCS#12 structure, or the password doesn't match, both `pemData` and the result come back empty — there is no error thrown, so check for an empty result rather than expecting a 4D error.

### Example

From the plugin's own test method (`TEST_PEM_FROM_P12.4dm`):

```4d
C_BLOB:C604($p12;$pemData;$details)
C_TEXT:C284($tPassword)

$tPassword:="Pass-123"  // Password for the P12 file
$fDoc:=Open document:C264("")  // the P12 file

If (OK=1)
  CLOSE DOCUMENT:C267($fDoc)
  
  DOCUMENT TO BLOB:C525(Document;$p12)
  
  $pemText:=PEM From P12 ($p12;$pemData;$tPassword;3)
  
  $fDoc:=Create document:C266("")
  If (OK=1)
    CLOSE DOCUMENT:C267($fDoc)
    BLOB TO DOCUMENT:C526(Document;$pemData)
  End if 
End if 
```

To get just the private key without any certificate details, pass `0` for `options`:

```4d
$pemText:=PEM From P12 ($p12;$pemData;$tPassword;0)
```

---

## Get_timestamp

**Get_timestamp → Text**

### Syntax

```4d
result := Get_timestamp
```

| Parameter | Type | Description |
|---|---|---|
| Result | Text | The current time in UTC, formatted `YYYY-MM-DDTHH:MM:SSZ`. |

### Description

Takes no parameters. The time is always UTC (`GMT`) regardless of the machine's local timezone — the command forces the process's `TZ` environment variable to `GMT` before formatting. This is a global, process-wide environment change made on every call; it isn't restored afterward, though since it's always set to the same value the net effect on other code reading local time via `TZ` is limited to "the timezone appears as GMT," not an arbitrary value.

### Example

```4d
$now:=Get_timestamp
ALERT:C41($now)  // e.g. "2026-08-12T14:03:21Z"
```

---

## Get_timestring

**Get_timestring → Text**

### Syntax

```4d
result := Get_timestring
```

| Parameter | Type | Description |
|---|---|---|
| Result | Text | The current time in UTC, formatted as an RFC-1123-style string (e.g. `Wed, 12 Aug 26 14:03:21 GMT`). |

### Description

Same UTC-forcing behavior as [Get_timestamp](#get_timestamp), with a different output format (`%a, %d %b %y %X %Z`) — useful for HTTP `Date`-style headers.

### Example

```4d
$httpDate:=Get_timestring
```

---

## Get_unixtime

**Get_unixtime → Text**

### Syntax

```4d
result := Get_unixtime
```

| Parameter | Type | Description |
|---|---|---|
| Result | Text | The current Unix time (seconds since 1970-01-01T00:00:00Z), as a decimal string. |

### Description

Takes no parameters. Returned as text, not a Longint, so you'll need `Val` if you need it numerically. Internally this is formatted through a 32-bit `int`, so on some future date the numeric range could be a limitation — not a near-term concern.

### Example

```4d
$epochText:=Get_unixtime
$epoch:=Val(C_LONGINT:C605(0);$epochText)
```

---

## MD5

**MD5 ( data ; encoding ) → Text**

### Syntax

```4d
result := MD5 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table in [Requirements & platform notes](#requirements--platform-notes). |
| Result | Text | The MD5 digest (16 bytes), encoded per `encoding`. |

### Description

Straightforward one-shot MD5. No streaming/incremental form is exposed.

### Example

```4d
CONVERT FROM TEXT:C1011("hello";"UTF-8";$data)
$digest:=MD5 ($data;0)  // hex
```

---

## RIPEMD160

**RIPEMD160 ( data ; encoding ) → Text**

### Syntax

```4d
result := RIPEMD160 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The RIPEMD-160 digest (20 bytes), encoded per `encoding`. |

### Example

```4d
CONVERT FROM TEXT:C1011("hello";"UTF-8";$data)
$digest:=RIPEMD160 ($data;0)
```

---

## SHA1

**SHA1 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA1 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-1 digest (20 bytes), encoded per `encoding`. |

### Example

```4d
CONVERT FROM TEXT:C1011("hello";"UTF-8";$data)
$digest:=SHA1 ($data;0)
```

---

## SHA224

**SHA224 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA224 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-224 digest (28 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`), hashing an empty string with `Crypto HEX`:

```4d
$source:=""
CONVERT FROM TEXT:C1011($source;"utf-8";$sourceData)

ASSERT:C1129(SHA224 ($sourceData;Crypto HEX)="d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f")
```

---

## SHA256

**SHA256 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA256 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-256 digest (32 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`):

```4d
$source:=""
CONVERT FROM TEXT:C1011($source;"utf-8";$sourceData)

ASSERT:C1129(SHA256 ($sourceData;Crypto HEX)="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
```

---

## SHA384

**SHA384 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA384 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-384 digest (48 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`):

```4d
ASSERT:C1129(SHA384 ($sourceData;Crypto HEX)="38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b")
```

---

## SHA512

**SHA512 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA512 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-512 digest (64 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`):

```4d
ASSERT:C1129(SHA512 ($sourceData;Crypto HEX)="cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")
```

---

## SHA512_224

**SHA512_224 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA512_224 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-512/224 digest (28 bytes) — a truncated SHA-512 variant, not the same as SHA-224 — encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`):

```4d
ASSERT:C1129(SHA512_224 ($sourceData;Crypto HEX)="6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4")
```

---

## SHA512_256

**SHA512_256 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA512_256 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA-512/256 digest (32 bytes) — a truncated SHA-512 variant, not the same as SHA-256 — encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA2.4dm`):

```4d
ASSERT:C1129(SHA512_256 ($sourceData;Crypto HEX)="c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a")
```

---

## RSASHA1

**RSASHA1 ( data ; privateKeyPEM ; encoding ) → Text**

### Syntax

```4d
result := RSASHA1 ( data ; privateKeyPEM ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The data to sign. It's hashed with SHA-1 internally, then signed — don't pre-hash it yourself. |
| `privateKeyPEM` | Blob | An unencrypted PEM-encoded RSA private key. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The RSA/SHA-1 signature, encoded per `encoding`. Empty if `privateKeyPEM` doesn't parse as a valid RSA private key. |

### Description

If `privateKeyPEM` fails to parse (wrong format, encrypted with a passphrase this command doesn't support, or not an RSA key at all), the result is silently empty — there's no 4D error raised.

### Example

```4d
CONVERT FROM TEXT:C1011("data to sign";"UTF-8";$data)
DOCUMENT TO BLOB:C525(Document;$privateKeyPEM)  // an unencrypted RSA private key .pem file
$signature:=RSASHA1 ($data;$privateKeyPEM;1)  // Base64
```

---

## RSASHA256

**RSASHA256 ( data ; privateKeyPEM ; encoding ) → Text**

### Syntax

```4d
result := RSASHA256 ( data ; privateKeyPEM ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The data to sign (hashed with SHA-256 internally). |
| `privateKeyPEM` | Blob | An unencrypted PEM-encoded RSA private key. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The RSA/SHA-256 signature, encoded per `encoding`. Empty on a key-parse failure. |

### Example

```4d
$signature:=RSASHA256 ($data;$privateKeyPEM;1)
```

---

## RSAVERIFYSHA1

**RSAVERIFYSHA1 ( data ; publicKeyPEM ; signature ; signatureEncoding ) → Longint**

### Syntax

```4d
result := RSAVERIFYSHA1 ( data ; publicKeyPEM ; signature ; signatureEncoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The original data that was signed. |
| `publicKeyPEM` | Blob | A PEM-encoded RSA public key (matching the private key that produced `signature`). |
| `signature` | Text | The signature to verify, in the encoding named by `signatureEncoding`. |
| `signatureEncoding` | Longint | `1` = Base64, anything else = hexadecimal. (Only these two are supported here — not the full 4-value encoding table used elsewhere in this plugin.) |
| Result | Longint | `1` if the signature is valid; `0` for any failure (bad signature, wrong key, unparseable key, malformed `signature` text). |

### Description

There's a single, undifferentiated failure result (`0`) for every possible failure mode — an invalid signature and a malformed public key both just come back `0`. If you need to distinguish "key didn't parse" from "signature didn't verify," you can't from this command's result alone.

### Example

```4d
$isValid:=RSAVERIFYSHA1 ($data;$publicKeyPEM;$signatureHex;0)
If ($isValid=1)
  // signature verified
End if 
```

---

## RSAVERIFYSHA256

**RSAVERIFYSHA256 ( data ; publicKeyPEM ; signature ; signatureEncoding ) → Longint**

### Syntax

```4d
result := RSAVERIFYSHA256 ( data ; publicKeyPEM ; signature ; signatureEncoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The original data that was signed. |
| `publicKeyPEM` | Blob | A PEM-encoded RSA public key. |
| `signature` | Text | The signature to verify. |
| `signatureEncoding` | Longint | `1` = Base64, anything else = hexadecimal. |
| Result | Longint | `1` if valid, `0` otherwise (see [RSAVERIFYSHA1](#rsaverifysha1) for the same single-result-code caveat). |

### Example

```4d
$isValid:=RSAVERIFYSHA256 ($data;$publicKeyPEM;$signatureB64;1)
```

---

## AES128

**AES128 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV ) → Text**

### Syntax

```4d
result := AES128 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The plaintext (to encrypt) or ciphertext (to decrypt). |
| `passphrase` | Blob | A passphrase, from which a key and IV are derived (`EVP_BytesToKey`, MD5, 2048 rounds). **Only used if `explicitKey` and `explicitIV` are both empty** — see Description. |
| `direction` | Longint | `0` = encrypt; any other value = decrypt. |
| `mode` | Longint | Cipher mode: `0`=ECB, `1`=CBC, `2`=CFB1, `3`=CFB8, `4`=CFB128, `5`=OFB, `6`=CTR, `7`=GCM **(see the GCM/CCM warning above — avoid)**, `8`=CCM **(same warning)**, `9`=XTS. Any other value defaults to ECB. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| `noPadding` | Longint | Non-zero disables PKCS padding (`EVP_CIPHER_CTX_set_padding(ctx, 0)`); `0` leaves padding on. |
| `explicitKey` | Blob | An exact key to use instead of deriving one from `passphrase`. Must be ≤ 16 bytes for AES-128 to avoid silent zero-padding (see the short-key warning above). Ignored entirely if `passphrase` is non-empty. |
| `explicitIV` | Blob | An exact IV to use instead of deriving one from `passphrase`. Same 16-byte/zero-padding caveat as `explicitKey`. Ignored entirely if `passphrase` is non-empty. |
| Result | Text | The encrypted or decrypted bytes, encoded per `encoding`. Empty if key/IV setup failed. |

### Description

`explicitKey`/`explicitIV` are **only consulted when `passphrase` is empty**. If `passphrase` is non-empty, the command derives its own key/IV from it and `explicitKey`/`explicitIV` are silently ignored, even if you populate them — this is why the plugin's own test passes empty blobs for these two parameters when using a passphrase (see below).

If key/IV setup fails for any reason (e.g. `explicitKey`/`explicitIV` supplied but exceed OpenSSL's internal maximum buffer sizes), the result is silently empty.

### Example

From the plugin's own test method (`TEST_AES.4dm`), encrypting with a passphrase, CBC mode, Base64 output, and padding enabled — note the comment documenting a version that used to crash:

```4d
//%attributes = {}
/*
https://discuss.4d.com/t/crash-on-using-aes128-and-aes256-in-plugin-commoncrypto/15770

AES128(&O;&O;&L;&L;&L;&L;&O;&O):T

*/

$vT_Password:="password"
CONVERT FROM TEXT:C1011($vT_Password;"UTF-8";$vx_Password)

C_BLOB:C604($blob;$placeHolder1;$placeHolder2)

  //correct result
$vT_Result:=AES128 ($blob;$vx_Password;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$placeHolder1;$placeHolder2)
```

Using an explicit key and IV instead of a passphrase (pass an empty blob for `passphrase` to activate this path):

```4d
C_BLOB:C604($emptyPassphrase;$key;$iv)
// $key must be exactly 16 bytes, $iv exactly 16 bytes, for AES-128
$ciphertext:=AES128 ($data;$emptyPassphrase;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$key;$iv)
```

---

## AES192

**AES192 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV ) → Text**

### Syntax

```4d
result := AES192 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV )
```

Same parameters as [AES128](#aes128), with a 24-byte key. **`mode = 9` (XTS) is not available for AES-192** — see the note in Requirements & platform notes; passing `9` silently falls back to ECB rather than erroring.

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | Plaintext or ciphertext. |
| `passphrase` | Blob | See [AES128](#aes128) — same precedence rule over `explicitKey`/`explicitIV`. |
| `direction` | Longint | `0` = encrypt, else decrypt. |
| `mode` | Longint | `0`–`8` as in AES128 (no `9`/XTS). |
| `encoding` | Longint | See the encoding table above. |
| `noPadding` | Longint | Non-zero disables padding. |
| `explicitKey` | Blob | Exact 24-byte key (ignored if `passphrase` non-empty). |
| `explicitIV` | Blob | Exact IV (ignored if `passphrase` non-empty). |
| Result | Text | Encrypted/decrypted output. |

### Example

```4d
$ciphertext:=AES192 ($data;$vx_Password;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$placeHolder1;$placeHolder2)
```

---

## AES256

**AES256 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV ) → Text**

### Syntax

```4d
result := AES256 ( data ; passphrase ; direction ; mode ; encoding ; noPadding ; explicitKey ; explicitIV )
```

Same shape as [AES128](#aes128), with a 32-byte key, and includes `mode = 9` (XTS).

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | Plaintext or ciphertext. |
| `passphrase` | Blob | See [AES128](#aes128). |
| `direction` | Longint | `0` = encrypt, else decrypt. |
| `mode` | Longint | `0`–`9`, including XTS. |
| `encoding` | Longint | See the encoding table above. |
| `noPadding` | Longint | Non-zero disables padding. |
| `explicitKey` | Blob | Exact 32-byte key (ignored if `passphrase` non-empty). |
| `explicitIV` | Blob | Exact IV (ignored if `passphrase` non-empty). |
| Result | Text | Encrypted/decrypted output. |

### Example

```4d
$ciphertext:=AES256 ($data;$vx_Password;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$placeHolder1;$placeHolder2)
```

---

## SHA3_224

**SHA3_224 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA3_224 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA3-224 digest (28 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA3.4dm`):

```4d
ASSERT:C1129(SHA3_224 ($sourceData;Crypto HEX)="6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7")
```

---

## SHA3_256

**SHA3_256 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA3_256 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA3-256 digest (32 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA3.4dm`):

```4d
ASSERT:C1129(SHA3_256 ($sourceData;Crypto HEX)="a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a")
```

---

## SHA3_384

**SHA3_384 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA3_384 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA3-384 digest (48 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA3.4dm`):

```4d
ASSERT:C1129(SHA3_384 ($sourceData;Crypto HEX)="0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004")
```

---

## SHA3_512

**SHA3_512 ( data ; encoding ) → Text**

### Syntax

```4d
result := SHA3_512 ( data ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The SHA3-512 digest (64 bytes), encoded per `encoding`. |

### Example

From the plugin's own test method (`TEST_SHA3.4dm`):

```4d
ASSERT:C1129(SHA3_512 ($sourceData;Crypto HEX)="a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26")
```

---

## SHAKE128

**SHAKE128 ( data ; length ; encoding ) → Text**

### Syntax

```4d
result := SHAKE128 ( data ; length ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `length` | Longint | Requested output size. Actual output bytes = `ceil(length / 4)`. `0` returns an empty result. Negative values are clamped to `0` (as of this review — see Requirements & platform notes). |
| `encoding` | Longint | **Currently ignored** — see below. |
| Result | Text | The SHAKE128 output, hex-encoded in virtually all cases (see Description). |

### Description

`length` is not a byte count — the actual number of output bytes is `ceil(length / 4)`. `encoding` is documented here for completeness, but as covered in [Requirements & platform notes](#requirements--platform-notes), it's currently not applied: the command's internal switch statement re-reads `length`'s raw numeric value where it should read `encoding` instead, so output is Base64 only if `length` happens to equal `1` or `2`, Base32 only if `length` happens to equal `3`, and hexadecimal for every other `length` value. In practice, for any realistic output size (more than a few bytes requested), expect hex regardless of what you pass for `encoding`.

### Example

From the plugin's own test method (`TEST_SHA3.4dm`):

```4d
$source:=""
CONVERT FROM TEXT:C1011($source;"utf-8";$sourceData)

ASSERT:C1129(SHAKE128 ($sourceData;0;Crypto HEX)="")
ASSERT:C1129(SHAKE128 ($sourceData;16;Crypto HEX)="7f9c2ba4")
ASSERT:C1129(SHAKE128 ($sourceData;32;Crypto HEX)="7f9c2ba4e88f827d")
ASSERT:C1129(SHAKE128 ($sourceData;64;Crypto HEX)="7f9c2ba4e88f827d616045507605853e")
ASSERT:C1129(SHAKE128 ($sourceData;256;Crypto HEX)="7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef263cb1eea988004b93103cfb0aeefd2a686e01fa4a58e8a3639ca8a1e3f9ae57e2")
```

---

## SHAKE256

**SHAKE256 ( data ; length ; encoding ) → Text**

### Syntax

```4d
result := SHAKE256 ( data ; length ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The bytes to hash. |
| `length` | Longint | Requested output size, in the same `length`-not-bytes convention as [SHAKE128](#shake128). For `length` values that are exact multiples of 4, actual output bytes = `length / 4`, matching SHAKE128 exactly. For non-multiples of 4, `SHAKE256` currently rounds up by more bytes than SHAKE128 does for the equivalent case — see Requirements & platform notes. Negative values are clamped to `0` (as of this review). |
| `encoding` | Longint | Currently ignored — same limitation as [SHAKE128](#shake128). |
| Result | Text | The SHAKE256 output, hex-encoded in virtually all cases. |

### Example

From the plugin's own test method (`TEST_SHA3.4dm`), with a length that's a multiple of 4 (so the rounding quirk above doesn't affect this particular result):

```4d
ASSERT:C1129(SHAKE256 ($sourceData;256;Crypto HEX)="46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be")
```

---

## HMACMD5

**HMACMD5 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACMD5 ( data ; key ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The message to authenticate. |
| `key` | Blob | The HMAC key. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The HMAC-MD5 tag, encoded per `encoding`. |

### Example

```4d
CONVERT FROM TEXT:C1011("message";"UTF-8";$data)
CONVERT FROM TEXT:C1011("secret";"UTF-8";$key)
$mac:=HMACMD5 ($data;$key;0)
```

---

## HMACSHA1

**HMACSHA1 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA1 ( data ; key ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `data` | Blob | The message to authenticate. |
| `key` | Blob | The HMAC key. |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The HMAC-SHA1 tag, encoded per `encoding`. |

### Example

```4d
$mac:=HMACSHA1 ($data;$key;0)
```

---

## HMACSHA224

**HMACSHA224 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA224 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA224 tag.

### Example

```4d
$mac:=HMACSHA224 ($data;$key;0)
```

---

## HMACSHA256

**HMACSHA256 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA256 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA256 tag — the most commonly used HMAC variant in this plugin.

### Example

```4d
CONVERT FROM TEXT:C1011("webhook payload";"UTF-8";$data)
CONVERT FROM TEXT:C1011("shared-secret";"UTF-8";$key)
$signature:=HMACSHA256 ($data;$key;1)  // Base64, e.g. for an HTTP header
```

---

## HMACSHA384

**HMACSHA384 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA384 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA384 tag.

### Example

```4d
$mac:=HMACSHA384 ($data;$key;0)
```

---

## HMACSHA512

**HMACSHA512 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA512 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA512 tag.

### Example

```4d
$mac:=HMACSHA512 ($data;$key;0)
```

---

## HMACSHA512_224

**HMACSHA512_224 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA512_224 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA512/224 tag.

### Example

```4d
$mac:=HMACSHA512_224 ($data;$key;0)
```

---

## HMACSHA512_256

**HMACSHA512_256 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA512_256 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA512/256 tag.

### Example

```4d
$mac:=HMACSHA512_256 ($data;$key;0)
```

---

## HMACSHA3_224

**HMACSHA3_224 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA3_224 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA3-224 tag.

### Example

```4d
$mac:=HMACSHA3_224 ($data;$key;0)
```

---

## HMACSHA3_256

**HMACSHA3_256 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA3_256 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA3-256 tag.

### Example

```4d
$mac:=HMACSHA3_256 ($data;$key;0)
```

---

## HMACSHA3_384

**HMACSHA3_384 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA3_384 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA3-384 tag.

### Example

```4d
$mac:=HMACSHA3_384 ($data;$key;0)
```

---

## HMACSHA3_512

**HMACSHA3_512 ( data ; key ; encoding ) → Text**

### Syntax

```4d
result := HMACSHA3_512 ( data ; key ; encoding )
```

Same parameter shape as [HMACSHA1](#hmacsha1). Result is the HMAC-SHA3-512 tag.

Note: HMAC does not support variable-length XOF digests, which is why there's no `HMACSHAKE128`/`HMACSHAKE256` in this plugin.

### Example

```4d
$mac:=HMACSHA3_512 ($data;$key;0)
```

---

## PBKDF2_HMAC_SHA1

**PBKDF2_HMAC_SHA1 ( password ; salt ; iterations ; encoding ) → Text**

### Syntax

```4d
result := PBKDF2_HMAC_SHA1 ( password ; salt ; iterations ; encoding )
```

| Parameter | Type | Description |
|---|---|---|
| `password` | Blob | The password/passphrase to derive a key from. |
| `salt` | Blob | The salt. |
| `iterations` | Longint | PBKDF2 iteration count. **No upper bound is enforced** — a very large value will make this call take a correspondingly long time, with no way to cancel it mid-call. Choose a value appropriate to your security/performance tradeoff (RFC 8018 and NIST guidance suggest values in the tens of thousands or higher for password storage as of this writing — verify current recommendations independently). |
| `encoding` | Longint | Output encoding — see the encoding table above. |
| Result | Text | The derived key (20 bytes, SHA-1's output size), encoded per `encoding`. |

### Description

This is the only PBKDF2 variant currently callable in this build — see [Requirements & platform notes](#requirements--platform-notes) for the other variants that exist in source but aren't wired up.

### Example

From the plugin's own test method (`TEST_PBKDF2.4dm`) — this reproduces a well-known PBKDF2-HMAC-SHA1 test vector with an embedded null byte in both the password and salt:

```4d
$iteration:=4096
CONVERT FROM TEXT:C1011("pass"+Char:C90(0)+"word";"us-ascii";$password)
CONVERT FROM TEXT:C1011("sa"+Char:C90(0)+"lt";"us-ascii";$salt)

$hash:=PBKDF2_HMAC_SHA1 ($password;$salt;$iteration;Crypto HEX)
  //56fa6aa75548099dcc37d7f03425e0c3 
  //9d35f8f9b26aa9a8aa39276634c6793a
```

A more typical case, deriving a key from a plain-text password:

```4d
CONVERT FROM TEXT:C1011("correct horse battery staple";"UTF-8";$password)
CONVERT FROM TEXT:C1011("a-random-per-user-salt";"UTF-8";$salt)
$derivedKey:=PBKDF2_HMAC_SHA1 ($password;$salt;100000;1)  // Base64
```

---

## PEMInfo

**PEMInfo ( pemData ) → Text**

### Syntax

```4d
result := PEMInfo ( pemData )
```

| Parameter | Type | Description |
|---|---|---|
| `pemData` | Blob | A PEM-encoded X.509 certificate. |
| Result | Text | A human-readable dump of the certificate's fields (issuer, subject, validity, etc.), the same format `openssl x509 -text` produces. Empty if `pemData` doesn't parse as a certificate. |

### Description

This is inspection/display output only — not structured data. If you need individual fields (expiry date, subject CN, etc.) programmatically, you'll need to parse this text yourself; there's no separate structured-output command.

### Example

```4d
DOCUMENT TO BLOB:C525(Document;$pemData)  // a .pem or .crt file
$info:=PEMInfo ($pemData)
ALERT:C41($info)
```

---

## DERToPEM

**DERToPEM ( derData ; pemData ) → Text**

### Syntax

```4d
result := DERToPEM ( derData ; pemData )
```

| Parameter | Type | Description |
|---|---|---|
| `derData` | Blob | A DER-encoded X.509 certificate. |
| `pemData` | Blob | *(by reference)* Receives the PEM-encoded conversion of the certificate. |
| Result | Text | **Not the PEM text** — a human-readable `PEMInfo`-style dump of the certificate's fields. The actual PEM-encoded certificate is written to `pemData`, not returned as text. If `derData` doesn't parse, both `pemData` and the result are left empty. |

### Description

The two output channels carry different content: `pemData` gets the actual PEM certificate (what you'd write to a `.pem` file); the function result gets a text summary of the certificate's contents, exactly like [PEMInfo](#peminfo). If you just want the PEM bytes, read `pemData`, not the result.

### Example

```4d
DOCUMENT TO BLOB:C525(Document;$derData)  // a .der or .cer file

C_BLOB:C604($pemData)
$info:=DERToPEM ($derData;$pemData)

$fDoc:=Create document:C266("")
If (OK=1)
  CLOSE DOCUMENT:C267($fDoc)
  BLOB TO DOCUMENT:C526(Document;$pemData)  // the actual PEM file
End if 

ALERT:C41($info)  // the human-readable summary, not the PEM itself
```

---

## Error handling & troubleshooting

- **Almost nothing here raises a 4D error.** Every command in this plugin fails silently — a bad password, an unparseable key, a malformed certificate, or invalid input all just produce an empty `Text` result (or `0` for the two `RSAVERIFY*` commands). Always check for an empty/zero result rather than expecting an error to catch.
- **`RSAVERIFYSHA1`/`RSAVERIFYSHA256` collapse every failure into the same `0`.** An invalid signature and an unparseable public key are indistinguishable from the result alone.
- **`AES128`/`AES192`/`AES256` GCM and CCM modes (`mode` 7/8) don't produce a real authentication tag.** Avoid these modes with this plugin; use CBC/CFB/OFB/CTR.
- **Short explicit AES keys/IVs are zero-padded, not rejected.** Always supply `explicitKey`/`explicitIV` at their exact required length.
- **`explicitKey`/`explicitIV` are ignored whenever `passphrase` is non-empty**, even if you populate them — pass an empty blob for `passphrase` to actually use an explicit key/IV.
- **`AES192` silently treats XTS mode (`9`) as ECB.** Only `AES128`/`AES256` actually support XTS.
- **`SHAKE128`/`SHAKE256`'s `encoding` parameter currently has no effect** — output is hex except by numeric coincidence. Don't rely on Base64/Base32 output from these two commands.
- **`SHAKE256`'s length rounding is slightly different from `SHAKE128`'s for non-multiple-of-4 lengths.** Request lengths that are multiples of 4 if you need predictable byte counts.
- **Only `PBKDF2_HMAC_SHA1` is exposed**, despite other PBKDF2 variants existing in the plugin's source.
- **`PBKDF2_HMAC_SHA1`'s `iterations` has no upper bound** — an excessive value will make the call take a very long time with no cancellation.
- **`Get_timestamp`/`Get_timestring` always return UTC**, regardless of the host machine's local timezone setting, by forcing the process's `TZ` environment variable to `GMT` on every call.
- **`DERToPEM`'s function result is a text summary, not the PEM bytes** — read the `pemData` output parameter for the actual PEM-encoded certificate.
- **`PEM From P12`'s actual command name has spaces**, unlike every other command in this plugin, which are named identically to their internal function names.

---

## Quick reference

```4d
// Hashing
$digest:=SHA256 ($data;0)  // hex
$digest:=SHA3_256 ($data;1)  // Base64

// HMAC
$mac:=HMACSHA256 ($data;$key;0)

// Key derivation
$key:=PBKDF2_HMAC_SHA1 ($password;$salt;100000;1)

// AES (passphrase-based; explicitKey/explicitIV must be empty blobs here)
C_BLOB:C604($emptyKey;$emptyIV)
$ciphertext:=AES256 ($plaintext;$passphrase;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$emptyKey;$emptyIV)
$plaintext:=AES256 ($ciphertext;$passphrase;Crypto Decrypt;Crypto AES CBC;Crypto BASE64;0;$emptyKey;$emptyIV)

// RSA sign / verify
$signature:=RSASHA256 ($data;$privateKeyPEM;1)
$isValid:=RSAVERIFYSHA256 ($data;$publicKeyPEM;$signature;1)

// Certificates
$info:=PEMInfo ($pemData)
$info:=DERToPEM ($derData;$pemData)  // PEM bytes land in $pemData, not $info
$pemText:=PEM From P12 ($p12Data;$pemData;$password;3)
```
