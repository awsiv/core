<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$handle = NULL;
$status = "x";
$regex = true;

$res = cfpr_hosts_compliance_for_promises($userName, $hostKey, $handle, $status, $regex, array(),array());

fwrite(STDERR, $res);
?>
