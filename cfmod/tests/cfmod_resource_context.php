<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_context($userName, $hostKey, null, 0, time(), 1, 50);
fwrite(STDERR, $res);
?>
