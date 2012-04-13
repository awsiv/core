<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostkey = NULL;
$hours_deltafrom = 0;
$hours_deltato = 0;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;

$res = cfpr_report_value($userName, $hostkey, NULL, NULL, NULL, array($class_regex), array(), 'day', true, $rows, $page_number);
fwrite(STDERR, $res);
?>
