<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_role_list($username); 
fwrite(STDERR, $res);
?>
