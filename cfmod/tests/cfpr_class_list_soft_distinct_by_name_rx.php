<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classRx = ".*";

$res = cfpr_class_list_soft_distinct_by_name_rx($userName, $classRx, 10, 1);
fwrite(STDERR, $res);
?>
