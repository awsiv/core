<?php
cfpr_header("control","normal");
?>

<div id="selector">

<table>
<tr>
<td>
<h2>Mission control and planning</h2>


<ul>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="formulate.php">Formulate promises</a>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="editor.php">Editor</a>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="services.php">Service catalogue</a>
<li><img src="48px-Crystal_Clear_app_network_2.png"><a href="new_topic.php">Add to knowledge bank</a>
</ul>

</td>
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

</td>
</tr>
</table>
</div>
     
<?php
cfpr_footer();
?>