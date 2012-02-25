<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$pid = 100;

$res = cfpr_show_topic_leads($userName, $pid);
fwrite(STDERR, $res);
?>
