<?php

$handle = $_POST['handle'];

if ($hostkey == "")
   {
   $handle = $_GET['handle'];
   }

$title = "promise $handle";
cfpr_header("$title","normal");

$pid = cfpr_get_pid_for_topic("promises","$handle");
?>


<table>
<tr>
<td>
<?php $allhandles = cfpr_list_handles(".*"); 
echo "$allhandles"; ?>
</td>
<td valign="top">
  <h2>Promise definition: </h2>

<p>

<?php 
$promise = cfpr_summarize_promise($handle);

echo "$promise<p>";

if ($pid > 0) 
   {
   $topic = cfpr_show_topic($pid);
   echo "<p>$topic";
   $leads = cfpr_show_topic_leads($pid);    
   echo "<p>$leads"; 
   }

?>
</td>
</tr>
</table>


<?php
cfpr_footer();
?>