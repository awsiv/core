<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");

$res = cfpr_host_list_by_ip_rx($userName, NULL, 15, 1);

?>
