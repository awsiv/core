<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$classIncludes = array();
$classExcludes = array();

$res = cfpr_summarize_repaired($userName, $hostKey, NULL, NULL, NULL, $classIncludes, $classExcludes, NULL, false, 0, 0);
fwrite(STDERR, $res);
?>
