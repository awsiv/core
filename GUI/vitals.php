<?php

$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];

if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

$view =  $_GET['view'];
$obs =  $_GET['obs'];
$nm =  $_GET['nm'];

switch ($view)
   {
   case 'mag':
     cfpr_header("vital signs","reload");
     echo "<img src=\"/hub/$hostkey/$nm"."_mag.png\">";
     $ret = cfpr_analysis_mag($hostkey,$obs);
     break;
   case 'week':
     cfpr_header("vital signs","normal");
     echo "<img src=\"/hub/$hostkey/$nm"."_week.png\">";
     $ret = cfpr_analysis_week($hostkey,$obs);
     break;
   case 'hist':
     cfpr_header("vital signs","normal");
     echo "<img src=\"/hub/$hostkey/$nm"."_hist.png\">";
     $ret = cfpr_analysis_hist($hostkey,$obs);
     break;
   default:
     cfpr_header("vital signs","normal");
     $ret = cfpr_performance_analysis($hostkey);
   }


echo "<br>$ret<br>";

cfpr_footer();
?>
          
