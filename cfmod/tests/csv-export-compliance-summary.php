<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$inclist = array('linux.*');
$exlist = array('');
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$version = NULL;
$promiseContext = null;

$test_path = "/tmp";
$filename = 'total-compliance' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_compliance_summary($userName, $hostKey, $promiseContext, $version, -1, -1, -1, -1, $inclist, $exlist, $sortCol, $sortDescending, $resultsPerPage, $pageNum, $export_info);

fwrite(STDERR, $res);


sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
