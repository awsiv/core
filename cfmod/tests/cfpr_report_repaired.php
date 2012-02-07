<?php
$userName = "admin";
$hostKey = NULL;
$handle = NULL;
$from = 0;
$to = 0;
$classRx = NULL;
$sortCol = "abc";
$sortDescending = true;
$resultsPerPage = 100;
$pageNum = 1;

$res = cfpr_report_repaired($userName, $hostKey, $handle, $from, $to, $classRx, $sortCol, $sortDescending, $resultsPerPage, $pageNum);

?>
