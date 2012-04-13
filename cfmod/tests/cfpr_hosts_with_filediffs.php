<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$classIncludes = array();
$classExcludes = array();
$cal = NULL;
$diff = NULL;
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_filediffs($userName, hostKey, $name, $diff, true, $cal, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
