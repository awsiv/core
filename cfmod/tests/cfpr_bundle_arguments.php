<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleType = "agent";
$bundleName = "cfengine_web_server";

$res = cfpr_bundle_arguments($userName, $bundleType, $bundleName);
fwrite(STDERR, $res);
?>
