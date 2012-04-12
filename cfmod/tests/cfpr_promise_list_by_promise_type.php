<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiseType = "files";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_promise_type($userName, $promiseType, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
