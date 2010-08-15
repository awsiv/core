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
?>
     
<table>
<tr>
<td>
<h2>Generate report:</h2>

 <form method="post" action="search.php">
<p><input class="searchfield" type="text" name="search" />&nbsp; search for .*?

<p><?php $allhosts = cfpr_select_hosts(".*",100);
echo "$allhosts";?>
&nbsp; on host
<p><?php $allreps = cfpr_select_reports(".*",100);
echo "$allreps";?>
&nbsp; in report
<p>
<input type="submit">

</form>

BUNDLES/PASSPORT?

</td>
<td>                                                                        


<?php
echo "<div id=\"info\">\n";

$last = cfpr_getlastupdate($hostkey);

echo "<div id=\"meter\"><a href=\"topN.php\"><img src=\"/hub/$hostkey/meter.png\" align=right></a></div>";


echo "<p><ul>";

$colour = cfpr_get_host_colour($hostkey);

echo "<li><a href=\"hosts.php?type=$colour\"><img src=\"/img/$colour"."_sign_big.png\"></a></li>\n";

echo "<li>$hostkey</li>";
echo "<li>Alias: $hostname</li>"; 
echo "<li>Last known address: $ipaddr</li>";
echo "<li>Last data were obtained at $last";
echo "<li><a href=\"vitals.php?hostkey=$hostkey\">Vital signs for this host</a></li>\n";
echo "</ul></div>\n";

cfpr_summary_meter(NULL);

?>

<form method="post" action="host.php">
<p><?php $allhosts = cfpr_select_hosts(".*",100);
echo "$allhosts";?>
<p>
<input type="submit">

</form>

</td>
<td>
<p>Host was last seen at .....
<p>Links to this host...
<p>Responsible for this host....
</td>
</tr>   
</table>


<?php

cfpr_footer();
?>