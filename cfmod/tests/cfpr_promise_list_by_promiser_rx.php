<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiserRx = ".*";

$res = cfpr_promise_list_by_promiser_rx($userName, $promiserRx);
fwrite(STDERR, $res);
?>
