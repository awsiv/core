<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_setuid($userName, $hostKey, $name, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
