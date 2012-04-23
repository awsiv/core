<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfmod_resource_host($userName, null, null, 0, time(), 1, 50);
fwrite(STDERR, $res);
?>
