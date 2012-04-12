<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiser = "cf-execd";
$resultsPerPage = 5;
$pageNum = 1;

$res = cfpr_promise_list_by_promiser($userName, $promiser, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
