<?php


$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];

if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

cfpr_header("vital signs","normal");

$ret = cfpr_performance_analysis($host);

echo "<br>$ret<br>";

cfpr_footer();
?>
          
