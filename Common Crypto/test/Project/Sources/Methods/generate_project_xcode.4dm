//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params)

$params:=$1

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$projectPath:=$path.parentFolder+$params.PRODUCT_NAME+".v9.xcodeproj"+Folder separator:K24:12
CREATE FOLDER:C475($projectPath;*)

$in:="v9"+Folder separator:K24:12+"project.xcodeproj"+Folder separator:K24:12+"project.pbxproj"
$out:=$params.PRODUCT_NAME+".v9.xcodeproj"+Folder separator:K24:12+"project.pbxproj"

generate_file_from_template ($1;$in;$out)

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$projectPath:=$path.parentFolder+$params.PRODUCT_NAME+".xcodeproj"+Folder separator:K24:12
CREATE FOLDER:C475($projectPath;*)

$in:="v10"+Folder separator:K24:12+"project.xcodeproj"+Folder separator:K24:12+"project.pbxproj"
$out:=$params.PRODUCT_NAME+".xcodeproj"+Folder separator:K24:12+"project.pbxproj"

generate_file_from_template ($1;$in;$out)
