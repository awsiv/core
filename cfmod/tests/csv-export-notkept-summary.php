<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$causeRx = ".*";
$from = 0;
$to = time();
$classIncludes = array('linux.*');
$classExcludes = array('');
$promise_context = NULL;

$test_path = "/tmp";
$filename = 'notkept-summary-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_summarize_notkept($userName, $hostKey,$promise_context, NULL, $causeRx, $from, $to, $classIncludes, $classExcludes, NULL, false, 0, 0, $export_info);

fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
