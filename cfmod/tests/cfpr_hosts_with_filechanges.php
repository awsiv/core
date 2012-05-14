<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;
$from = 0;
$to = time(NULL);

$res = cfpr_hosts_with_filechanges($userName, $hostKey, $name, true, $from, $to, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
