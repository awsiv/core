<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$search = ".*";
$classIncludes = array();
$classExcludes = array();
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$promiseContext = null;

$res = cfpr_report_classes($userName, $hostKey, $promiseContext, $search, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, 0, time(null), $resultsperPage, $pageNum);

fwrite(STDERR, $res);
?>
