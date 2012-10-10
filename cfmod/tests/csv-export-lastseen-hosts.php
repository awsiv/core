<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$key = NULL;
$name = NULL;
$address = NULL;
$ago = 0;
$classIncludes = array('linux.*');
$classExcludes = array('');
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$promiseContext = null;

$test_path = "/tmp";
$filename = 'lastseen-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_lastseen($userName, $hostKey, $promiseContext, $key, $name, $address, $ago, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsperPage, $pageNum, $export_info);

fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
