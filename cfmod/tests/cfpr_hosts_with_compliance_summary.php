<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_compliance_summary($userName, $hostKey, -1, -1, -1, -1, ">", $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
