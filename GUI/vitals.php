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
$obs_name = cfpr_get_observable_name($obs);
$nm =  $_GET['nm'];

$host = cfpr_hostname($hostkey);

switch ($view)
   {
   case 'mag':
     cfpr_header("vital signs","reload");
     echo "<h1>Past 4 hours' $obs_name on $host</h1>";
     echo "<table><tr><td valign=\"top\">";    
     echo "<div id=\"image\">";
     echo "<img src=\"/hub/$hostkey/$nm"."_mag.png\">";
     $ret = cfpr_analysis_mag($hostkey,$obs);
     break;
   case 'week':
     cfpr_header("vital signs","normal");
     echo "<h1>Past week's $obs_name on $host</h1>";
     echo "<table><tr><td valign=\"top\">";    
     echo "<div id=\"image\">";
     echo "<img src=\"/hub/$hostkey/$nm"."_week.png\">";
     $ret = cfpr_analysis_week($hostkey,$obs);
     break;
   case 'hist':
     cfpr_header("vital signs","normal");
     echo "<h1>Frequency spectrum for $obs_name on $host</h1>";
     echo "<table><tr><td valign=\"top\">";    
     echo "<div id=\"image\">";
     echo "<img src=\"/hub/$hostkey/$nm"."_hist.png\">";
     $ret = cfpr_analysis_hist($hostkey,$obs);
     break;
   default:
     cfpr_header("vital signs","normal");
     $ret = cfpr_performance_analysis($hostkey);
   }

switch ($view)
   {
   case 'mag':
   case 'week':
   case 'hist':
     echo "</td><td width=\"40%\" valign=\"top\"><h2>Legend</h2><br><br><a href=\"hosts.php?type=green\"><img src=\"/img/green.png\"></a><br><br>Indicates the identification of a maximum (peak) value, to the maximum accuracy of the data ccategories.<br><br><a href=\"hosts.php?type=red\"><img src=\"/img/red.png\"></a><br><br>Indicates the position of the global average value.<br><br><a href=\"hosts.php?type=red\"><img src=\"/img/blue.png\"></a><br><br>Normalized frequency of values relative to the mean.</td></tr>";
     echo "</td></tr></table>";
     break;
   default:

   }

echo "<br>$ret<br>";


cfpr_footer();
?>
          
