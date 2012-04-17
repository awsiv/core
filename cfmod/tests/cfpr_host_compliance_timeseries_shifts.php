<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classIncludes = array();
$classExcludes = array();

$res = cfpr_host_compliance_timeseries_shifts($userName, $classIncludes, $classExcludes);
fwrite(STDERR, $res);
?>
