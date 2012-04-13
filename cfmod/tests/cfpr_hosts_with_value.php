<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$days = "";
$months = "";
$years = "";
$classIncludes = array();
$classExcludes = array();
$rows = 100;
$page_number = 1;

$res = cfpr_hosts_with_value($userName, $hostKey, $days, $months, $years, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
