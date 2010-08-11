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


<ul>

<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Bundle%20profile">bundles report</a>   Bundles used on this host 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Classes">classes report</a>   User defined classes observed on the system 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Compliance%20report">compliance report</a>   Total summary of host compliance 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=File%20changes%20report">file_changes report</a>   Latest observed changes to system files 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=File%20%diffs%20report">file_diffs report</a>   Latest observed differences to system files 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Last%20saw%20hosts">lastseen report</a>   Time and frequency of communications with peers 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Patches%20available">patches available report</a>   Patches already installed on system 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Patch%20status">patches installed report</a>   Patches not yet installed, but published by vendor 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Performance">performance report</a>   Time cost of verifying system promises 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Compliance by promise">promise report</a>   Per-promise average compliance report 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Promises%20not%20kept">promises not kept report</a>   Promises that were recently unkept 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Promises repaired">promises repaired report</a>   Promises that were recently repaired 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Setuid/gid root programs">setuid report</a>   Known setuid programs found on system 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Software installed">software installed report</a>   Software already installed on system 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Business value report">value report</a>   Value estimate / ROI of cfengine configuration 
<li><a href="search.php?hostkey=<?php echo $hostkey;?>&report=Variables">variables report</a> Values of set and discovered variables on this host context
</ul>

</td>
<td>                                                                        


<?php
echo "<div id=\"info\">\n";

echo "<ul>";
echo "<li><img src=\"/img/green_sign_big.png\"></li>\n";
echo "<li>Key: $hostkey</li>";
echo "<li>Alias: $hostname</li>"; 
echo "<li>Last known address: $ipaddr</li>";
echo "<li><a href=\"vitals.php\">Vital signs for this host</a></li>\n";
echo "<li><a href=\"magview.php\">Last four hours</a></li>\n";
echo "<li><a href=\"year.php\">Long term trends</a></li>\n";
echo "<li><a href=\"weekly.php\">Patterns</a></li>\n";
echo "</ul></div>\n";

cfpr_summary_meter(NULL);

echo "<div id=\"meter\"><a href=\"topN.php\"><img src=\"/hub/$hostkey/meter.png\"></a></div>";
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