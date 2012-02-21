<?php

$userName = getenv("CFENGINE_TEST_USERNAME");

$res = cfpr_environment_list($userName);
fwrite(STDERR, implode("\n", $res));

?>
