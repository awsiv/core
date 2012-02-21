<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_class_list_all($userName, 5, 1);
fwrite(STDERR, $res);
?>
