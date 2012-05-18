<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$inclist = array();
$exlist = array();
$sortCol = "last-seen";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;
$version = NULL;

$res = cfpr_report_compliance_summary($userName, $hostKey, $version, -1, -1, -1, -1, $inclist, $exlist, $sortCol, $sortDescending, $resultsPerPage, $pageNum);

fwrite(STDERR, $res);
?>
