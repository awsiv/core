<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$causeRx = ".*";
$classIncludes = array();
$classExcludes = array();

$res = cfpr_summarize_notkept($userName, $hostKey, NULL, $causeRx, NULL, NULL, $classIncludes, $classExcludes, NULL, false, 0, 0);
fwrite(STDERR, $res);
?>
