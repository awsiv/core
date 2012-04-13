<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");$name = "";
$class_regex = "";
$rows = 20;
$page_number = 1;

$res = cfpr_report_performance($userName, $hostkey, $name, true, array($class_regex), array(), "last-performed", true, $rows, $page_number);
fwrite(STDERR, $res);

?>
