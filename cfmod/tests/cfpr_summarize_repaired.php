<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$causeRx = ".*";
$from = 0;
$to = time();
$classIncludes = array();
$classExcludes = array();

$res = cfpr_summarize_repaired($userName, $hostKey, NULL, $causeRx, $from, $to, $classIncludes, $classExcludes, NULL, false, 0, 0);
fwrite(STDERR, $res);
?>
