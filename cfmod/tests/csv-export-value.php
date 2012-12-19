<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostkey = NULL;
$hours_deltafrom = 0;
$hours_deltato = 0;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;

$test_path = "/tmp";
$filename = 'business-value-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_value($userName, $hostkey, NULL, NULL, NULL, array($class_regex), array(''), 'day', true, $rows, $page_number, $export_info);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>