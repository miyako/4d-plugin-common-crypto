//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params)

$params:=$1

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$lprojPath:=$path.parentFolder+"English.lproj"+Folder separator:K24:12
CREATE FOLDER:C475($lprojPath;*)

$in:="InfoPlist.strings"
$out:="English.lproj"+Folder separator:K24:12+"InfoPlist.strings"

generate_file_from_template ($1;$in;$out)