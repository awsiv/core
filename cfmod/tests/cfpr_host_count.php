<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$colour = "blue";

$res = cfpr_host_count($userName, $colour, array(), array());
fwrite(STDERR, $res);
?>
