//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1)
C_TEXT:C284($2;$3;$fileName)
C_BOOLEAN:C305($4;$withBom)

$path:=Path to object:C1547(Get 4D folder:C485(Database folder:K5:14))
$templatesFolder:=$path.parentFolder+"templates"+Folder separator:K24:12

$params:=$1

$fileName:=$2

$filePath:=$templatesFolder+$fileName

If (Test path name:C476($filePath)=Is a document:K24:1)
	
	$template:=Document to text:C1236($filePath;"utf-8")
	PROCESS 4D TAGS:C816($template;$template;$params)
	
	$fileName:=$3
	$filePath:=$path.parentFolder+$fileName
	
	$withBom:=Bool:C1537($4)
	
	If (Test path name:C476($filePath)=Is a document:K24:1)
		DELETE DOCUMENT:C159($filePath)
	End if 
	
	If (Test path name:C476($filePath)=Is a folder:K24:2)
		DELETE FOLDER:C693($filePath;Delete with contents:K24:24)
	End if 
	
	If ($withBom)
		TEXT TO DOCUMENT:C1237($filePath;$template;"utf-8";Document with CRLF:K24:20)
	Else 
		CONVERT FROM TEXT:C1011($template;"utf-8";$data)
		BLOB TO DOCUMENT:C526($filePath;$data)
	End if 
	
End if 