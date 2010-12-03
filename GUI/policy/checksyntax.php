<?php
session_start();
$path=realpath('../policies/'.session_id().'/promises.cf');
if(file_exists($path))
{
$result=cfpr_validate_policy($path);
}
else
{
$result="Failed to find promises.cf";
}

$details=array(
		'result'=>$result
	     );
echo json_encode($details);
?>