<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classIncludes = array('.*');
$classExcludes = array();

$res = cfpr_astrolabe_host_list($userName, $classIncludes, $classExcludes);
fwrite(STDERR, $res);
?>
