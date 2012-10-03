<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostkey = NULL;
$rows = 9999;
$page_number = 1;

$res = cfpr_report_setuid($userName, $hostkey, NULL, false, array(), array(), null, false, $rows, $page_number);
fwrite(STDERR, $res);
?>
