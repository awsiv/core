<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostkey = NULL;
$rows = 9999;
$page_number = 1;

$test_path = "/tmp";
$filename = 'setuid-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_setuid($userName, $hostkey, NULL, false, array('linux.*'), array(''), null, false, $rows, $page_number, $export_info);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
