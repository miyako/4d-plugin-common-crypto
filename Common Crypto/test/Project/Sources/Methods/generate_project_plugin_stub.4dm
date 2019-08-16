//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params)

$params:=$1

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$templatesFolder:=$path.parentFolder+"templates"+Folder separator:K24:12

  //create target 
$targetPath:=$path.parentFolder+\
"test"+Folder separator:K24:12+"Plugins"+Folder separator:K24:12+\
$params.PRODUCT_NAME+".bundle"+Folder separator:K24:12+"Contents"+Folder separator:K24:12
CREATE FOLDER:C475($targetPath;*)

CREATE FOLDER:C475($targetPath+"MacOS"+Folder separator:K24:12;*)
CREATE FOLDER:C475($targetPath+"QuickLook"+Folder separator:K24:12;*)
CREATE FOLDER:C475($targetPath+"Resources"+Folder separator:K24:12+"English.lproj"+Folder separator:K24:12;*)
CREATE FOLDER:C475($targetPath+"Windows"+Folder separator:K24:12;*)
CREATE FOLDER:C475($targetPath+"Windows64"+Folder separator:K24:12;*)

  //stubs
$stubPath:=$targetPath+"Windows"+Folder separator:K24:12+$params.PRODUCT_NAME+".4DX"
If (Test path name:C476($stubPath)#Is a document:K24:1)
	COPY DOCUMENT:C541($templatesFolder+"Windows"+Folder separator:K24:12+"plugin.4DX";$stubPath)
End if 

$stubPath:=$targetPath+"Windows64"+Folder separator:K24:12+$params.PRODUCT_NAME+".4DX"
If (Test path name:C476($stubPath)#Is a document:K24:1)
	COPY DOCUMENT:C541($templatesFolder+"Windows64"+Folder separator:K24:12+"plugin.4DX";$stubPath)
End if 

$stubPath:=$targetPath+"MacOS"+Folder separator:K24:12+$params.PRODUCT_NAME
If (Test path name:C476($stubPath)#Is a document:K24:1)
	COPY DOCUMENT:C541($templatesFolder+"MacOS"+Folder separator:K24:12+"plugin";$stubPath)
End if 

  //icon
$iconPath:=$targetPath+"QuickLook"+Folder separator:K24:12+"Thumbnail.png"
If (Test path name:C476($iconPath)#Is a document:K24:1)
	COPY DOCUMENT:C541($templatesFolder+"Thumbnail.png";$iconPath)
End if 

generate_constants_xlf ($params)

$projectFolder:=$path.parentFolder

  //constants.xlf
$constantsXlfPath:=$projectFolder+"constants.xlf"
$constantsPath:=$targetPath+"Resources"+Folder separator:K24:12+"constants.xlf"
COPY DOCUMENT:C541($constantsXlfPath;$constantsPath;*)

  //manifest.json
$manifestJsonPath:=$projectFolder+"manifest.json"
$manifestPath:=$targetPath+"manifest.json"
COPY DOCUMENT:C541($manifestJsonPath;$manifestPath;*)

generate_project_infoplist ($params)

  //infoPlist.strings
$infoPlistStringsPath:=$projectFolder+"English.lproj"+Folder separator:K24:12+"InfoPlist.strings"
$infoPlistPath:=$targetPath+"Resources"+Folder separator:K24:12+"English.lproj"+Folder separator:K24:12+"InfoPlist.strings"
COPY DOCUMENT:C541($infoPlistStringsPath;$infoPlistPath;*)

$in:="Info.plist"
$out:="test"+Folder separator:K24:12+"Plugins"+Folder separator:K24:12+\
$params.PRODUCT_NAME+".bundle"+Folder separator:K24:12+"Contents"+Folder separator:K24:12+"Info.plist"

generate_file_from_template ($1;$in;$out)

$in:="Info.plist"
$out:="Info.plist"

generate_file_from_template ($1;$in;$out)


