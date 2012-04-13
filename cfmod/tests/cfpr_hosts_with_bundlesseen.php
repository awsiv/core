<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_bundlesseen($userName, NULL, $name, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
