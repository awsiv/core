<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$class_regex = 'linux.*';
$cal = NULL;
$diff = NULL;
$rows = 200;
$page_number = 1;
$search = NULL;

$res =  cfpr_report_filediffs($userName, NULL, $search, $diff, true, $cal, array(), array(), "time", true, $rows, $page_number);
fwrite(STDERR, $res);

?>
