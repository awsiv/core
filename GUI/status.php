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
<ul><li>
<h2>Report on multiple hosts:</h2>

 <form method="post" action="search.php">
  <p>Search in report:</p>
<p><?php $allreps = cfpr_select_reports(".*",100);
echo "$allreps";?>
<p>
<input type="submit" value="Formulate query">
</form>
<li><h2>Examine a specific host</h2>
<form method="post" action="host.php">
<p><?php $allhosts = cfpr_select_hosts(".*",100);
echo "$allhosts";?>
<p>
<input type="submit">
</form>



</li>
</ul>

<td>
<h2>Average compliance of all hosts</h2>
<div id="meter"><a href="topN.php"><img src="/hub/common/meter.png"></a></div>
<ul>
<li>Hosts known: <?php echo $all; ?>
<li><a href="hosts.php?type=red"><img src="/img/red_sign.png"></a>     <?php echo "$r"; ?> hosts known
<li><a href="hosts.php?type=yellow"><img src="/img/yellow_sign.png"></a>  <?php echo "$y"; ?> hosts known
<li><a href="hosts.php?type=green"><img src="/img/green_sign.png"></a>   <?php echo "$g"; ?> hosts known
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