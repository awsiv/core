<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;
$complianceLevel = "red";

$res = cfpr_hosts_with_lastknown_bundlesseen($userName, NULL, $complianceLevel, $name, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
