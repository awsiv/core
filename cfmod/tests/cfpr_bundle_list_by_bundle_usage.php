<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleName = "cfengine_update_folders";

$res = cfpr_bundle_list_by_bundle_usage($userName, $bundleName);

?>
