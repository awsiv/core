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
<div id="noborder">
<table>
<tr><td>

<h2>Hosts</h2>

<ul>
<li><i>Hosts known:</i> <?php echo $all; ?>
<li><i>Ranked worst:</i> <a href="topN.php">Show top 50</a>
</ul>

</td><tr>
<tr><td>

<h2>Status</h2>
<ul>
<li><a href="hosts.php?type=red"><img class="icontext" src="/img/red_sign_small.png"></a>     <?php echo "$r"; ?> hosts known
<li><a href="hosts.php?type=yellow"><img class="icontext" src="/img/yellow_sign_small.png"></a>  <?php echo "$y"; ?> hosts known
<li><a href="hosts.php?type=green"><img class="icontext" src="/img/green_sign_small.png"></a>   <?php echo "$g"; ?> hosts known
<li>Average download time: 0.0 s
<li>Hosts 2 dev overdue: 0
</ul>

</td></tr>
</table>
</div>
<td>
<div id="noborder">

<table>
<tr><td>
<h2>Average meters for all hosts</h2>

<ul>
<li><div id="meter"><a href="knowledge.php?topic=Status level meters"><img src="/hub/common/meter.png"></a></div>
<li><img class="icontext" src="/img/dive.png"> <a href="host.php">Select a particular host</a>


</ul>

</td></tr>
<tr><td>

<h2>Search groups of hosts</h2>

<ul><li>
 <form method="post" action="search.php">
<p><?php $allreps = cfpr_select_reports(".*",100);
echo "$allreps";?>
<p>
<input type="submit" value="step 1 of 2">
</ul>
</form>


</td></tr>
</table>
</div>
</td>

</tr>
</table>
</div>

<?
cfpr_footer();
?>