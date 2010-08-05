<?php

cfpr_header("vital signs","normal");

$hostkey = NULL;
$name = ".*";
$regex = 1;

//$ret = cfpr_report_classes(NULL,$name,true);
//$ret = cfpr_report_software_in(NULL,NULL,NULL,NULL,0);

$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;

$host = "MD5=8863e92018e2651280be79d88b2cd122";

$ret = cfpr_performance_analysis($host);


echo "<br>$ret<br>";

cfpr_footer();
?>
          
