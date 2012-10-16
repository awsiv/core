<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$lval = '';
$scope = 'sys';
$promiseContext = null;

$test_path = "/tmp";
$filename = 'variables-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);
$res = cfpr_report_vars($userName, $hostKey, $promiseContext, $scope, $lval, NULL, NULL, false, array('linux.*'), array(''), 'var-name', true, 1, 1, $export_info);

fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
