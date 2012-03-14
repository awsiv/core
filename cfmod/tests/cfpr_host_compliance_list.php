<?php
$userName = getenv("CFENGINE_TEST_USERNAME");

$colour = 'blue';
$includes = array('.*');
$excludes = array();

$resultsPerPage = 100;
$pageNum = 1;

$res = cfpr_host_compliance_list($userName, $colour, $includes, $excludes, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);
?>
