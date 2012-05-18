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
$hostColour = NULL;
$status = 'x'; // any


$res = cfpr_report_lastknown_compliance_promises($userName, $hostKey, $hostColour, $handle, $status, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsPerPage, $pageNum);

fwrite(STDERR, $res);
?>
