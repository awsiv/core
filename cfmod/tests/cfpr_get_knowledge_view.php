<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$pid = 100;
$view = "";

$res = cfpr_get_knowledge_view($userName, $pid, $view);
fwrite(STDERR, $res);
?>
