<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_promise_log_notkept_summary($userName, null, null, $hostKey, null, 0, time(), 1, 50);
fwrite(STDERR, $res);
?>
