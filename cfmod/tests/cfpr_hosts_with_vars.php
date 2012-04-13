<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$scope = NULL;
$lval = NULL;
$rval = NULL;
$type = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_vars($userName, $hostKey, $scope, $lval, $rval, $type, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
