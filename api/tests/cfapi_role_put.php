<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_role_put($username, 'jersey', 'dummy role', '.*', null, null, null); 
fwrite(STDERR, $res);

$res = cfapi_role_post($username, 'jersey', 'dummy2', null, null, null, null);
fwrite(STDERR, $res);

?>
