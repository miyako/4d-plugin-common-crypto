//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params)

$params:=$1

$in:="project.vcxproj"
$out:=$params.PRODUCT_NAME+".vcxproj"

generate_file_from_template ($1;$in;$out;True:C214)

$in:="project.sln"
$out:=$params.PRODUCT_NAME+".sln"

generate_file_from_template ($1;$in;$out;True:C214)

$in:="project.vcxproj.filters"
$out:=$params.PRODUCT_NAME+".vcxproj.filters"

generate_file_from_template ($1;$in;$out;True:C214)
