<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$handleRx = ".*";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_handle_rx($userName, $handleRx, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
