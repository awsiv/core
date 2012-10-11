<?php
$userName =  getenv("CFENGINE_TEST_USERNAME");
$hostKey = NULL;
$search = ".*";
$classIncludes = array('linux.*');
$classExcludes = array('');
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$promiseContext = null;

$test_path = "/tmp";
$filename = 'classes-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_classes($userName, $hostKey, $promiseContext, $search, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, 0, time(null), $resultsPerPage, $pageNum, $export_info);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
