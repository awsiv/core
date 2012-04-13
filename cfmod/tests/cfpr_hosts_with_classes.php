<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$version = NULL;
$arch = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_classes($userName, NULL, $name, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
