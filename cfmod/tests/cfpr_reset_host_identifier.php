<?php

$out = <<<EOS
{
  "hostIdentifier": ""
}
EOS;

$user = getenv("CFENGINE_TEST_USERNAME");

$ret = cfpr_reset_host_identifier($user);

if($ret)
{
  $ret_json = cfpr_get_host_identifier($user);	
}
else
{
	fwrite(STDERR, "ERROR: failed resetting host identifier");
}

if( $out == $ret_json )
{
	fwrite(STDERR, "PASS");
}
else
{
	fwrite(STDERR, "ERROR: Incorrect value after 'cfpr_reset_host_identifier'");
}

?>