<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiserRx = ".*";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_promiser_rx($userName, $promiserRx, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
