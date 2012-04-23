<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfmod_resource_software($userName, $hostKey, null, null, null, null, 1, 50);
fwrite(STDERR, $res);
?>
