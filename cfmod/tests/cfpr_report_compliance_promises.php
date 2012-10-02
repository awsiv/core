<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$handle = NULL;
$classIncludes = array();
$classExcludes = array();
$sortCol = "abc";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$status = 'x'; // any
$promiseContext = null;

$res = cfpr_report_compliance_promises($userName, $hostKey, $promiseContext, $handle, $status, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsPerPage, $pageNum);

fwrite(STDERR, $res);
?>
