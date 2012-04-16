<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$handle = NULL;
$causeRx = ".*";
$from = 0;
$to = time();
$classIncludes = array();
$classExcludes = array();
$sortCol = "abc";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;

$res = cfpr_report_repaired($userName, $hostKey, $handle, $causeRx, $from, $to, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
