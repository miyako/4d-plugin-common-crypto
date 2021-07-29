//%attributes = {}
/*
https://discuss.4d.com/t/crash-on-using-aes128-and-aes256-in-plugin-commoncrypto/15770

AES128(&O;&O;&L;&L;&L;&L;&O;&O):T

*/

$vT_Password:="password"
CONVERT FROM TEXT:C1011($vT_Password;"UTF-8";$vx_Password)

C_BLOB:C604($blob;$placeHolder1;$placeHolder2)

  //empty result
  //https://www.openssl.org/docs/man1.0.2/man3/EVP_CIPHER_CTX_set_padding.html
$vT_Result:=AES128 ($blob;$vx_Password;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;Crypto AES No padding;$placeHolder1;$placeHolder2)

  //correct result
$vT_Result:=AES128 ($blob;$vx_Password;Crypto Encrypt;Crypto AES CBC;Crypto BASE64;0;$placeHolder1;$placeHolder2)
