<?php

$userName = getenv("CFENGINE_TEST_OVERRIDE_USERNAME");

$res = cfpr_bundle_agent_goals($userName);

?>
