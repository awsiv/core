<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$class_regex = 'linux.*';
$from = 0;
$to = time(NULL);
$diff = NULL;
$rows = 200;
$page_number = 1;
$search = NULL;
$promise_context = NULL;

$test_path = "/tmp";
$filename = 'filediffs-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res =  cfpr_report_filediffs($userName, NULL, $promise_context, $search, $diff, true, $from, $to, array(''), array(''), "time", true, $rows, $page_number, $export_info);

fwrite(STDERR, $res);

sleep(1);
// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
