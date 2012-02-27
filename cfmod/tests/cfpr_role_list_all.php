<?php

$username = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_role_list_all($username);
fprintf(STDERR, $res);
?>
