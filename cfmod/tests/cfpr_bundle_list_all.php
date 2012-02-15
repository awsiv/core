<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");

$res = cfpr_bundle_list_all($userName);

?>
