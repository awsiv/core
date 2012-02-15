<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$handleRx = ".*";

$res = cfpr_promise_list_by_handle_rx($userName, $handleRx);

?>
