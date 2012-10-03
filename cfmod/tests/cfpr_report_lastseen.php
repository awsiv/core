<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$key = NULL;
$name = NULL;
$address = NULL;
$ago = -1;
$classIncludes = array();
$classExcludes = array();
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$promiseContext = null;

$res = cfpr_report_lastseen($userName, $hostKey, $promiseContext, $key, $name, $address, $ago, true, $classIncludes, $classExcludes, $sortCol, $sortDescending, $resultsperPage, $pageNum);

fwrite(STDERR, $res);
?>
