<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$colour = "blue";
$classIncludes = array();
$classExcludes = array();

$res = cfpr_host_count($userName, $colour, $classIncludes, $classExcludes);
fwrite(STDERR, $res);
?>
