<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$classRx = "ent.*";

$res = cfpr_class_list_all($userName, $classRx, 10, 1);
fwrite(STDERR, $res);
?>
