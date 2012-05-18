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

$res = cfpr_report_classes($userName, $hostKey, $search, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsperPage, $pageNum);

fwrite(STDERR, $res);
?>
