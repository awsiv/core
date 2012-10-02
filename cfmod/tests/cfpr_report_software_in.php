<?php
$userName = getenv("CFENGINE_TEST_USERNAME");
$hostKey = getenv("CFENGINE_TEST_HOSTKEY");
$name = NULL;
$version = NULL;
$arch = NULL;
$regex = false;
$resultsPerPage = 10;
$pageNum = 1;
$promiseContext = null;

$res =  cfpr_report_software_in($userName, $hostKey, $promiseContext, $name, $version, $arch, $regex, array(), array(), "sortnone", false, $resultsPerPage, $pageNum);
fwrite(STDERR, $res);

?>
