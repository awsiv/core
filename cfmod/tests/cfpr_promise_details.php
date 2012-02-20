<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiseHandle = "promise_promises_cf_107";

$res = cfpr_promise_details($userName, $promiseHandle);
fwrite(STDERR, $res);
?>
