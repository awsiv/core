<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$bundle = NULL;
$regex = true;

$res = cfpr_hosts_compliance_for_bundles($userName, $hostKey, $bundle, $regex, array(),array());

fwrite(STDERR, $res);
?>
