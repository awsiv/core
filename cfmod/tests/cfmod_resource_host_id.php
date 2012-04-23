<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_host_id($userName, $hostKey);
fwrite(STDERR, $res);
?>
