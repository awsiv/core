<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_host_compliance_count_green($userName);

?>
