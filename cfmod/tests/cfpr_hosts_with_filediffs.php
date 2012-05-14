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
$from = 0;
$to = time(NULL);

$res = cfpr_hosts_with_filediffs($userName, hostKey, $name, $diff, true, $from, $to, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
