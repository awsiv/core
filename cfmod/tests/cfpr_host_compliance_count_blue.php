<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");

$res = cfpr_host_compliance_count_blue($userName);

?>
