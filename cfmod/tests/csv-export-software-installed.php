<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = NULL;
$name = NULL;
$version = NULL;
$arch = NULL;
$regex = false;
$resultsPerPage = 10;
$pageNum = 1;
$promisecontext = NULL;

$test_path = "/tmp";
$filename = 'software-installed-' . time();
$reportinfo_array = array('path'=>$test_path,'filename'=>$filename,'request_id'=>'asdf', 'csv' => true, 'pdf' => false)

$res =  cfpr_report_software_in($userName, $hostKey, $promisecontext, $name, $version, $arch, $regex, array('linux.*'), array(''), "sortnone", false, $resultsPerPage, $pageNum, $reportinfo_array);
fwrite(STDERR, $res);

sleep(1);

// CLEANUP

$csv_file = $test_path . "/" . $filename . ".csv";
$status_file = $csv_file . ".status";

unlink($csv_file);
unlink($status_file);
?>
