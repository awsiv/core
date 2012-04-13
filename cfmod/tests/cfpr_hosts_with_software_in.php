<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$version = NULL;
$arch = NULL;
$classIncludes = array();
$classExcludes = array();
$rows = 200;
$page_number = 1;

$res = cfpr_hosts_with_software_in($userName, $hostKey, $name, $version, $arch, true, $classIncludes, $classExcludes, $rows, $page_number);
fwrite(STDERR, $res);

?>
