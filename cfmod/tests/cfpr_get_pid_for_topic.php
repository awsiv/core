<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$type = "";
$topic = "system policy";

$res = cfpr_get_pid_for_topic($userName, $type, $topic);
fwrite(STDERR, $res);
?>
