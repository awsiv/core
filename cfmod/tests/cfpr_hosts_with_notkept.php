<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$hours_deltafrom = 0;
$hours_deltato = 0;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_notkept($userName, $hostKey, $name, intval($hours_deltafrom), intval($hours_deltato), $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
