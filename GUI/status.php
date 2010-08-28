<?php
cfpr_header("engineering status","normal");

$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();

?>

<div id="selector">

<table>
<tr>
<td valign="top">
<table>
<tr><td>

<?php echo "<span id=\"colourbuttons\"><a href=\"hosts.php?type=green\"><img src=\"/img/green.png\"></a> <a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow.png\"></a> <a href=\"hosts.php?type=red\"><img src=\"/img/red.png\"></a></span>"; ?>
<h2>Hosts:</h2>

<ul>
<li><i>Hosts known:</i> <?php echo $all; ?>
<li><i>Ranked worst:</i> <a href="topN.php">Top 50</a>
</ul>

</td><tr>
<tr><td>

<h2>Status</h2>
<ul>
<li><a href="hosts.php?type=red"><img src="/img/red_sign_small.png"></a>     <?php echo "$r"; ?> hosts known
<li><a href="hosts.php?type=yellow"><img src="/img/yellow_sign_small.png"></a>  <?php echo "$y"; ?> hosts known
<li><a href="hosts.php?type=green"><img src="/img/green_sign_small.png"></a>   <?php echo "$g"; ?> hosts known
<li>Average download time: 0.0 s
<li>Hosts 2 dev overdue: 0
</ul>

</td></tr>
</table>

<td>

<table>
<tr><td>

<h2>Average meters for all hosts</h2>

<ul>
<li><div id="meter"><a href="knowledge.php?topic=Status level meters"><img src="/hub/common/meter.png"></a></div>
<li><img src="/img/dive.png"> <a href="host.php">Select a particular host</a>


</ul>

</td></tr>
<tr><td>

<h2>Report on all hosts:</h2>

 <form method="post" action="search.php">
  <p>Search in report:</p>
<p><?php $allreps = cfpr_select_reports(".*",100);
echo "$allreps";?>
<p>
<input type="submit" value="Formulate query">
</form>


</td></tr>
</table>
</td>

</tr>
</table>
</div>
     
<?
cfpr_footer();
?>