<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_host_count_all($userName);
fwrite(STDERR, $res);
?>
