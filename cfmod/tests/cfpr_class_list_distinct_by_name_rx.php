<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classRx = null;
$includes = array("ent.*");
$excludes = array();

$res = cfpr_class_list_distinct_by_name_rx($userName, $classRx, $includes, $excludes, 10, 1);
fwrite(STDERR, $res);
?>
