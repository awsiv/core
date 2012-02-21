<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfpr_environment_by_hostkey($userName, $hostKey);
fwrite(STDERR, $res);
?>
