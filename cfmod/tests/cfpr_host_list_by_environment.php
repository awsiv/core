<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$environment = "production";

$arr = cfpr_host_list_by_environment($userName, $environment);
fwrite(STDERR, implode("\n", $arr));
?>
