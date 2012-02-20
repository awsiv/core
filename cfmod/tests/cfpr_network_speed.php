<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfpr_network_speed($userName, $hostKey);
fwrite(STDERR, $res);
?>
