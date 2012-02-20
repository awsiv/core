<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleType = "agent";
$bundleName = "cfengine_limit_robot_agents";

$res = cfpr_bundle_classes_used($userName, $bundleType, $bundleName);

?>
