<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$includes = array(".*");
$excludes = array();

$res = cfpr_class_list_soft_distinct_by_name_rx($userName, $includes, $excludes, 10, 1);
fwrite(STDERR, $res);
?>
