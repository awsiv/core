<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_user_delete($username, 'jwow'); 
fwrite(STDERR, $res);
?>
