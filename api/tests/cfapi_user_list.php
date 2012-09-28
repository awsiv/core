<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_user_list($username, null, null, 1, 2000); 
fwrite(STDERR, $res);
?>
