<?php

$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];

if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

$hostname = cfpr_hostname($hostkey);
$ipaddr = cfpr_ipaddr($hostkey);
$title = "host $hostname";
cfpr_header("$title","normal");
cfpr_host_meter($hostkey);
$colour = cfpr_get_host_colour($hostkey);


?>
     
<table>
<tr>
<td>
<div id="status">

<?php 
echo "<p><form method=\"post\" action=\"host.php\">";
$allhosts = cfpr_select_hosts(".*",100);
echo "$allhosts";
echo "<input type=\"submit\">";
echo "</form></h2>";
echo "<p>";

echo "  <h2>Selected: $hostname</h2>";
echo "<p><a href=\"hosts.php?type=$colour\"><img src=\"/img/$colour"."_sign_big.png\"></a><br></p>\n"; 

$last = cfpr_getlastupdate($hostkey);
$class = cfpr_get_variable($hostkey,"sys","ostype");
$flavour = cfpr_get_variable($hostkey,"sys","flavour");
$rel = cfpr_get_variable($hostkey,"sys","release");
$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
$speed = cfpr_get_network_rate($hostkey);

echo "<p><ul>";
#echo "<li>$hostkey</li>";
echo "<li><i>Alias</i>: <b>$hostname</b></li>"; 
echo "<li><i>Operating system class</i>: <b>$class</b></li>";
echo "<li><i>Release</i>: <b>$rel</b></li>";
echo "<li><i>Flavour</i>: <b>$flavour</b></li>";
echo "<li><i>Last known address</i>: <b>$ipaddr</b></li>";
echo "<li><i>Last data</i>: <b>$last</b></li>";
echo "<li><i>Average load</i>: <b>$load%</b></li>";
echo "<li><i>Average free on system disk</i>: <b>$free%</b> </li>";
echo "<li><i>Average network speed</i>: $speed</li>";
echo "</ul></div>\n";

?>

</div>
</td>
<td>                                                                        

<?php
echo "<div id=\"info\">\n";

cfpr_summary_meter(NULL);

echo "<h2>Average compliance of $hostname</h2>";

echo "<div id=\"meter\"><a href=\"topN.php\"><img src=\"/hub/$hostkey/meter.png\"></a></div>";

echo "<ul>";
echo "<li><img src=\"/img/vital.png\">&nbsp;<a href=\"vitals.php?hostkey=$hostkey\">Pulse and vital signs for this host</a></li>\n";

echo "<li><form method=\"post\" action=\"search.php\">";
echo "Search: (.*+[])";

echo "<p><input class=\"searchfield\" type=\"text\" name=\"search\" />";

echo "<p><input type=\"hidden\" name=\"hostkey\" value=\"$hostkey\">";

$allreps = cfpr_select_reports(".*",100);

echo "$allreps";
echo "<p>";
echo "<input type=\"submit\">";
echo "</form>";
echo "</ul>";

$number = cfpr_get_class_frequency(NULL,"mXC.*");
$nlist = cfpr_report_class_frequency($hostkey,"mXC.*");
echo "Monitored jobs globally: $number<br>$nlist";

?>
</form>
</td>
</tr>   
</table>


<?php
$report = cfpr_summarize_notkept($hostkey,NULL);
echo "<h4>Promises not kept in the past week</h4> $report";

# Takes too long
#$report = cfpr_summarize_repaired($hostkey,NULL);
#echo "<h4>Promises repaired in the past week</h4> $report";

cfpr_footer();
?>
