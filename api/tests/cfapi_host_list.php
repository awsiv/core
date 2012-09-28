<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_host_list($username, array(), array(), 1, 1000); 
fwrite(STDERR, $res);
?>
