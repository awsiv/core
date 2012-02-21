<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_class_list_all($userName);
fwrite(STDERR, $res);
?>
