<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$lval = 'av_loadavg';
$scope = 'mon';
$promiseContext = null;

$res = cfpr_report_vars($userName, $hostKey, $promiseContext, $scope, $lval, NULL, NULL, false, array(), array(), 'var-name', true, 1, 1);
fwrite(STDERR, $res);

?>
