<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$causeRx = '.*';
$from = 0;
$to = time();
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;
$promise_context = NULL;

$test_path = "/tmp";
$filename = 'hostonly-notkept-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_hosts_with_notkept($userName, $hostKey, $promise_context, $name, $causeRx, intval($from), intval($to), $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

// CLEANUP
sleep(1);

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
