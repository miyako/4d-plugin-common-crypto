//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params;$json)

$params:=$1

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$templatesFolder:=$path.parentFolder+"templates"+Folder separator:K24:12
$constantsXlfPath:=$templatesFolder+"constants.xlf"
$constantsXlf:=Document to text:C1236($constantsXlfPath;"utf-8")
$projectFolder:=$path.parentFolder
$constantsJsonPath:=$projectFolder+"constants.json"
$json:=JSON Parse:C1218(Document to text:C1236($constantsJsonPath;"utf-8"))
PROCESS 4D TAGS:C816($constantsXlf;$constantsXlf;$params;$json)
$constantsXlfPath:=$projectFolder+"constants.xlf"
TEXT TO DOCUMENT:C1237($constantsXlfPath;$constantsXlf;"utf-8";Document with CRLF:K24:20)