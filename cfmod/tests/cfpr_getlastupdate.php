<?php
$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$hostKey = getenv("CFENGINE_TEST_OVERRIDE_HOSTKEY");

$res = cfpr_getlastupdate($userName, $hostKey); 
?>
