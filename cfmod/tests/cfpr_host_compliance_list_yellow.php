<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$resultsPerPage = 100;
$pageNum = 1;

$res = cfpr_host_compliance_list_yellow($userName, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
