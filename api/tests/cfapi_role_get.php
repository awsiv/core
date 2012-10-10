<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_user_get($username, 'admin'); 
fwrite(STDERR, $res);
?>
