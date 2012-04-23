<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_variable($userName, $hostKey, null, null, null, null, null, 0, time(), 1, 50);
fwrite(STDERR, $res);
?>
