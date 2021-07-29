//%attributes = {}

  // Method: TEST_PEM_FROM_P12
  // Tests PEM From P12 plugin method.
  // 
  // 

  // Created by: Alistair Bates (29/7/21)
  // 

  // Parameters
  // ----------------------------------------------------


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