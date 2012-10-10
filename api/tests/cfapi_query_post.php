<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$query = "SELECT * FROM Hosts ORDER BY HostKey";

$res = cfapi_query_post($username, $query, null, false, -1, -1); 
fwrite(STDERR, $res);
?>
