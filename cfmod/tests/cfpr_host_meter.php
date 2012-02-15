<?php
$username = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$hostkey = getenv("CFENGINE_TEST_OVERRIDE_HOSTKEY");

$res = cfpr_host_meter($username, $hostkey); 
?>
