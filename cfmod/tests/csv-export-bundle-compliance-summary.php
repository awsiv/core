<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;
$keyhash = NULL;
$complianceLevel = 'green';
$promiseContext = null;

$test_path = "/tmp";
$filename = 'bundlecompliance-summary-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_lastknown_bundlesseen($userName, $keyhash, $complianceLevel, $promiseContext, $name, true, array($class_regex), array(), "last-verified", true, $rows, $page_number, $export_info);

fwrite(STDERR, $res);

// wait for files to be created
// Assuming less than 1 sec is taken to  generate the files

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
