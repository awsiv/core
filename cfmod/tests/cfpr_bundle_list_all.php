<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_bundle_list_all($userName);

?>
