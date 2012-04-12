<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleType = "agent";
$bundleName = "garbage_collection";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_bundle($userName, $bundleType, $bundleName, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
