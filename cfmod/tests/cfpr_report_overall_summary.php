<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$handle = NULL;
$status = "x";
$regex = false;
$classRx = NULL;

$res = cfpr_report_overall_summary($userName, $hostKey, $handle, $status, $regex, $classRx);
fwrite(STDERR, $res);
?>
