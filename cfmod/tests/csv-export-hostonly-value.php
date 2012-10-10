<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$days = "";
$months = "";
$years = "";
$classIncludes = array('');
$classExcludes = array('');
$rows = 100;
$page_number = 1;

$test_path = "/tmp";
$filename = 'hostonly-value-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_hosts_with_value($userName, $hostKey, $days, $months, $years, $classIncludes, $classExcludes, $rows, $page_number, $export_info);
fwrite(STDERR, $res);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
