<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_host_compliance_count_red($userName);
fwrite(STDERR, $res);
?>
