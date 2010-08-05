<?php
cfpr_header("engineering status","normal");

$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();

?>

<div id="selector">



<table border="0">
<tr>
<td>
<ul>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="topN.php">Status all hosts</a>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="host.php">Status by host</a>
<li><img src="48px-Crystal_Clear_app_network_2.png">Scan for hosts with property
<li><img src="48px-Crystal_Clear_app_network_2.png">Summarize compliance
<li><img src="48px-Crystal_Clear_app_network_2.png">Summarize performance
<li><img src="48px-Crystal_Clear_app_network_2.png">Summarize business goals
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="index.php">Management summary</a>
</ul>
</td>
<td>
<h2>Average Compliance</h2>
<div id="meter"><a href="topN.php"><img src="/hub/common/meter.png"></a></div>
<ul>
<li>Hosts known: <?php echo $all; ?>
<li><img src="/img/red_sign.png">     <?php echo "$r"; ?> hosts known
<li><img src="/img/yellow_sign.png">  <?php echo "$y"; ?> hosts known
<li><img src="/img/green_sign.png">   <?php echo "$g"; ?> hosts known
<li>Average download time: 0.0 s
<li>Hosts 2 dev overdue: 0
</ul>





</td>
</tr>
</table>

</div>
     
<?

cfpr_footer();
?>