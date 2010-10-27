<?php
session_start();
$path=realpath('../policies/'.session_id().'/promises.cf');
$result=cfpr_validate_policy($path);

$details=array(
		'result'=>$result
	     );
echo json_encode($details);
?>