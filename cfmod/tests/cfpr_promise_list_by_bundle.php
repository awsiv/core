<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleType = "agent";
$bundleName = "garbage_collection";

$res = cfpr_promise_list_by_bundle($userName, $bundleType, $bundleName);

?>
