<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$bundleNameRx = ".*";

$res = cfpr_promise_list_by_bundle_rx($userName, $bundleNameRx);
fwrite(STDERR, $res);
?>
