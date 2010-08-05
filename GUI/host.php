<?php

cfpr_header("classes","normal");

$host = "MD5=8863e92018e2651280be79d88b2cd122";
$name = cfpr_hostname($host);
$title = "host $name";
?>
     
<table>
<tr>
<td>
<h2>Search:</h2>

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
<li><a href="">classes report</a>   User defined classes observed on the system 
<li><a href="">compliance report</a>   Total summary of host compliance 
<li><a href="">file_changes report</a>   Latest observed changes to system files 
<li><a href="">file_diffs report</a>   Latest observed differences to system files 
<li><a href="">lastseen report</a>   Time and frequency of communications with peers 
<li><a href="">patches available report</a>   Patches already installed on system 
<li><a href="">patches installed report</a>   Patches not yet installed, but published by vendor 
<li><a href="">performance report</a>   Time cost of verifying system promises 
<li><a href="">promise report</a>   Per-promise average compliance report 
<li><a href="">promises not kept report</a>   Promises that were recently unkept 
<li><a href="">promises repaired report</a>   Promises that were recently repaired 
<li><a href="">setuid report</a>   Known setuid programs found on system 
<li><a href="">software installed report</a>   Software already installed on system 
<li><a href="">value report</a>   Value estimate / ROI of cfengine configuration 
<li><a href="">variables report</a>
</ul>

</td>
<td>                                                                        
<?php
echo "<div id=\"info\"><ul>\n";
echo "<li><img src=\"/img/green_sign_big.png\">\n";
echo "<li><a href=\"vitals.php\">Vital signs for this host</a>\n";
echo "<li><a href=\"magview.php\">Last four hours</a>\n";
echo "<li><a href=\"year.php\">Long term trends</a>\n";
echo "<li><a href=\"weekly.php\">Patterns</a>\n";
echo "</ul></div>\n";

cfpr_summary_meter(NULL);

echo "<div id=\"meter\"><a href=\"topN.php\"><img src=\"/hub/$host/meter.png\"></a></div>";
?>
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