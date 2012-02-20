<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$vitalId = "cpu";

$res = cfpr_vitals_analyse_magnified($userName, $hostKey, $vitalId); 
?>
