<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_host_id_seenby($userName, $hostKey, 0, time(), 1, 50);
fwrite(STDERR, $res);
?>
