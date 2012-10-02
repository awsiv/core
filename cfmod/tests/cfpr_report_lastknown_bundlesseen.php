<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;
$keyhash = NULL;
$complianceLevel = 'red';
$promiseContext = null;

$res = cfpr_report_lastknown_bundlesseen($userName, $keyhash, $complianceLevel, $promiseContext, $name, true, array($class_regex), array(), "last-verified", true, $rows, $page_number);
fwrite(STDERR, $res);
?>
