<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$hours_deltafrom = 0;
$hours_deltato = 0;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;
$keyhash = NULL;

$res = cfpr_report_bundlesseen($userName, $keyhash, $name, true, array($class_regex), array(), "last-verified", true, $rows, $page_number);
fwrite(STDERR, $res);
?>
