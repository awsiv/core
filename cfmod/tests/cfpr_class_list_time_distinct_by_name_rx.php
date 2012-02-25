<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classRx = "Q.*";

$res = cfpr_class_list_time_distinct_by_name_rx($userName, $classRx, 10, 1);
fwrite(STDERR, $res);
?>
