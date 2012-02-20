<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$promiseType = "files";

$res = cfpr_promise_list_by_promise_type($userName, $promiseType);
fwrite(STDERR, $res);
?>
