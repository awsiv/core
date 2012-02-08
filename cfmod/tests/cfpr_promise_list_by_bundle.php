<?php

$userName = "admin";
$bundleType = "agent";
$bundleName = "garbage_collection";

$res = cfpr_promise_list_by_bundle($userName, $bundleType, $bundleName);

?>
