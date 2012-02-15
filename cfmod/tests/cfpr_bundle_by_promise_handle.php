<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$promiseHandle = "change_management_vars_watch_files";

$res = cfpr_bundle_by_promise_handle($userName, $promiseHandle);

?>
