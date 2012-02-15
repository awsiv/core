<?php
$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$hostKey = getenv("CFENGINE_TEST_OVERRIDE_HOSTKEY");

$res = cfpr_vitals_list($userName, $hostKey); 
?>
