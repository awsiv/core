<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$pid = 1;

$res = cfpr_show_topic($userName, $pid);
fwrite(STDERR, $res);
?>
