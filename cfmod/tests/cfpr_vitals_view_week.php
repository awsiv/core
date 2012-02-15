<?php
$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$hostKey = getenv("CFENGINE_TEST_OVERRIDE_HOSTKEY");
$vitalId = "cpu";

$res = cfpr_vitals_view_week($userName, $hostKey, $vitalId); 
?>
