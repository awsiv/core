<?php
//$filename=$_POST['file']||$_GET['file'];
$path=realpath('../policies/promises.cf');
$result=cfpr_validate_policy($path);
$details=array(
		'result'=>$result
	     );
echo json_encode($details);
?>