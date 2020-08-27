//%attributes = {}
$iteration:=4096
CONVERT FROM TEXT:C1011("pass"+Char:C90(0)+"word";"us-ascii";$password)
CONVERT FROM TEXT:C1011("sa"+Char:C90(0)+"lt";"us-ascii";$salt)

$hash:=PBKDF2_HMAC_SHA1 ($password;$salt;$iteration;Crypto HEX)
  //56fa6aa75548099dcc37d7f03425e0c3
