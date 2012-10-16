<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = "";
$class_regex = "";
$rows = 20;
$page_number = 1;

$test_path = "/tmp";
$filename = 'hostonly-performance-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_hosts_with_performance($userName, $hostkey, $name, true, array($class_regex), array(), $rows, $page_number, $export_info);
fwrite(STDERR, $res);

// CLEANUP
sleep(1);

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
