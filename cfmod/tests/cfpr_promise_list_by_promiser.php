<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");
$promiser = "cf-execd";

$res = cfpr_promise_list_by_promiser($userName, $promiser);

?>
