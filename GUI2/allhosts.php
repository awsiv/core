<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("all hosts","normal");

$hostkey = NULL;
$name = ".*";
$regex = 1;
$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;

$ret = cfpr_show_all_hosts($name,1000);

echo "<br>$ret<br>";

cfpr_footer();
?>
          
