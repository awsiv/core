<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$handle = NULL;
$causeRx = ".*";
$from = 0;
$to = time();
$classIncludes = array('linux.*');
$classExcludes = array();
$sortCol = "abc";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$promiseContext = null;

$test_path = "/tmp";
$filename = 'repaired-log-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_repaired($userName, $hostKey, $promiseContext, $handle, $causeRx, $from, $to, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsPerPage, $pageNum, $export_info);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
