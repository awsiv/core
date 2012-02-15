<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$promiseHandle = "promise_promises_cf_107";

$res = cfpr_promise_details($userName, $promiseHandle);

?>
