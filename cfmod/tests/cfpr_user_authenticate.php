<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfpr_user_authenticate($username, $password);
fwrite(STDERR, $res);
?>
