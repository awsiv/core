<?php

$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];

if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

if ($hostkey == "")
   {
   $hostkey = "none";
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
<td colspan="2">

<?php 

echo "<form method=\"post\" action=\"host.php\">";
$allhosts = cfpr_select_hosts($hostkey,".*",100);
echo "$allhosts";
echo "<input type=\"submit\" value=\"select host\">";
echo "<span id=\"colourbuttons\"><a href=\"hosts.php?type=green\"><img src=\"/img/green.png\"></a> <a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow.png\"></a> <a href=\"hosts.php?type=red\"><img src=\"/img/red.png\"></a></span>";
echo "</form>";
?>
</td></tr><tr><td valign="top">

<?php
$last = cfpr_getlastupdate($hostkey);
$class = cfpr_get_variable($hostkey,"sys","ostype");
$flavour = cfpr_get_variable($hostkey,"sys","flavour");
$rel = cfpr_get_variable($hostkey,"sys","release");
$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
$speed = cfpr_get_network_rate($hostkey);

echo "<div id=\"status\">";

echo "<table><tr><td valign=\"top\">";
echo "<h2>Host</h2>";
echo "<p><ul>";
#echo "<li>$hostkey</li>";
echo "<li><i>Alias</i>: <b>$hostname</b></li>"; 
echo "<li><i>Operating system class</i>: <b>$class</b></li>";
echo "<li><i>Release</i>: <b>$rel</b></li>";
echo "<li><i>Flavour</i>: <b>$flavour</b></li>";
echo "<li><i>Last known address</i>: <b>$ipaddr</b></li>";
echo "<li><i>Last data</i>: <b>$last</b></li>";
echo "<li><i>ID</i>: <span id=\"keyhash\">$hostkey</soan>";
echo "</ul>";
echo "</td></tr><tr><td valign=\"top\">";

echo "<a href=\"#notkept\"><img src=\"/img/$colour"."_sign_big.png\"></a>&nbsp;\n"; 
echo "<h2>Status</h2>";
echo "<ul>";
echo "<li><i>Average load</i>: <b>$load%</b></li>";
echo "<li><i>Average free on system disk</i>: <b>$free%</b> </li>";
echo "<li><i>Average network speed</i>: $speed</li>";
echo "</ul>\n";
echo "</td></tr></table>";
echo "</div>";
?>

</td>
<td valign="top">                                                                        
<?php

echo "<table><tr><td valign=\"top\">";

cfpr_summary_meter(NULL);

echo "<h2>Analysis</h2>";

echo "<div id=\"meter\">";
echo "<ul>";
echo "<li><a href=\"knowledge.php?topic=Status level meters\"><img src=\"/hub/$hostkey/meter.png\"></a></li>";
echo "<li><a href=\"vitals.php?hostkey=$hostkey\"><img class=\"icontext\" src=\"/img/dive.png\"> &nbsp;Pulse and vital signs for this host</a></li>\n";
echo "</ul>";
echo "</div>";
echo "</td></tr><tr><td>";

echo "<h2>Generate report</h2>";

echo "simple search string:<br>";
echo "<form method=\"post\" action=\"search.php\">";
echo "<p><input class=\"searchfield\" type=\"text\" name=\"search\" />";

echo "<p><input type=\"hidden\" name=\"hostkey\" value=\"$hostkey\">";

$allreps = cfpr_select_reports(".*",100);

echo "$allreps";
echo "<p>";
echo "<input type=\"submit\" value=\"commit\">";
echo "</form>";
echo "</ul>";

echo "</td></tr><tr><td>";

$number = cfpr_get_class_frequency(NULL,"mXC.*");
$nlist = cfpr_report_class_frequency($hostkey,"mXC.*");

echo "<h2>Monitored jobs</h2> <p><i>Total number under surveillance:</i> $number<br><i>On this host: </i><br>$nlist";

echo "</td></tr></table>";
?>
</td>
</tr>   
</table>

<?php
echo "<div id=\"notkeptinline\">";
$report = cfpr_summarize_notkept($hostkey,NULL);
echo "<a name=\"notkept\"><h4>Promises not kept in the past week</h4></a>";
echo "$report";
echo "<div>";

cfpr_footer();
?>
