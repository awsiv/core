<?php
$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$resultsPerPage = 100;
$pageNum = 1;

$res = cfpr_host_compliance_list_all($userName, $resultsPerPage, $pageNum);

?>
