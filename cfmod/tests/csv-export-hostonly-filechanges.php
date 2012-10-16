<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$classIncludes = array('');
$classExcludes = array('');
$rows = 200;
$page_number = 1;
$from = 0;
$to = time(NULL);
$promise_context = NULL;

$test_path = "/tmp";
$filename = 'hostonly-filechanges-' . time();
$export_info = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false);

$res = cfpr_hosts_with_filechanges($userName, $hostKey, $promise_context, $name, true, $from, $to, $classIncludes, $classExcludes, $rows, $page_number, $export_info);
fwrite(STDERR, $res);

// CLEANUP
sleep(1);

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);

?>
