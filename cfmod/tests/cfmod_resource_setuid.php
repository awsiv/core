<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_setuid($userName, $hostKey, null, null, 1, 50);
fwrite(STDERR, $res);
?>
