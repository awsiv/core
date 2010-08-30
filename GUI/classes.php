<?php
cfpr_header("classes","normal");

$hostkey = NULL;
$name = ".*";
$regex = 1;

//$ret = cfpr_report_classes(NULL,$name,true);
//$ret = cfpr_report_software_in(NULL,NULL,NULL,NULL,0);

$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;

//$ret = cfpr_report_lastseen($hostkey,$hash,$host,$addr,$tago,$regex);

$ret = cfpr_report_classes($hostkey,$name,$regex);

echo "<br>$ret<br>";

cfpr_footer();
?>
