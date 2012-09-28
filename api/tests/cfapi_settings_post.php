<?php
$username = getenv("CFENGINE_TEST_USERNAME");
$password = getenv("CFENGINE_TEST_PASSWORD");

$res = cfapi_settings_post($username, '{
		"ldapEnabled": false,
                "ldapMode": "standard",
                "ldapHost": "localhost",
                "ldapPort": 1025,
                "ldapBaseDN": "dc=localhost",
                "ldapLoginAttribute": "uid",
                "ldapUsersDirectory": "ou=people",
                "ldapEncryption": "plain",
                "ldapUsername": "snookie",
                "ldapPassword": "pass",
		"logLevel": "debug",
		"blueHostHorizon": 3
}'); 
fwrite(STDERR, $res);
?>
