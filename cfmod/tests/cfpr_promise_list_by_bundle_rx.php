<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleTypeRx = "a.*";
$bundleNameRx = ".*";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_bundle_rx($userName, $bundleTypeRx, $bundleNameRx, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
