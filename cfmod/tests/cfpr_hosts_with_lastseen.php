<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$key = NULL;
$name = NULL;
$address = NULL;
$ago = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_lastseen($userName, $hostKey, $key, $name, $address, $ago, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
