<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$causeRx = '.*';
$from = 0;
$to = time();
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_notkept($userName, $hostKey, $name, $causeRx, intval($from), intval($to), $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
