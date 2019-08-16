//%attributes = {"invisible":true,"preemptive":"capable"}
C_OBJECT:C1216($1;$params)

$params:=$1

$in:="main.h"
$out:="4DPlugin-"+Replace string:C233($params.PRODUCT_NAME;" ";"-")+".h"

generate_file_from_template ($1;$in;$out;True:C214)

$in:="main.cpp"
$out:="4DPlugin-"+Replace string:C233($params.PRODUCT_NAME;" ";"-")+".cpp"

generate_file_from_template ($1;$in;$out;True:C214)