<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_user_put($username, 'user123', 'pass', 'User', 'a@b.com', array('admin')); 
fwrite(STDERR, $res);
?>
