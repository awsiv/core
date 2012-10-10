<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$name = NULL;
$hours_deltafrom = 0;
$hours_deltato = 0;
$class_regex = 'linux.*';
$rows = 10;
$page_number = 1;
$keyhash = NULL;
$promiseContext = null;

$test_path = "/tmp";
$filename = 'bundle-compliance-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_report_bundlesseen($userName, $keyhash, $promiseContext, $name, true, array($class_regex), array(''), "last-verified", true, $rows, $page_number, $export_info);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
