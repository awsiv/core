<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");

$res = cfpr_host_info($userName, $hostKey); 
fwrite(STDERR, $res);
?>