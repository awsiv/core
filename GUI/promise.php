<?php

$handle = $_POST['handle'];

if ($hostkey == "")
   {
   $handle = $_GET['handle'];
   $expand = $_GET['expand'];
   }

$title = "promise $handle";
cfpr_header("$title","normal");

$pid = cfpr_get_pid_for_topic("promises","$handle");
?>

<div id="showpromise">
<table>
<tr>
<td valign="top">
<?php 
$mybundle = cfpr_get_promise_bundle($handle);
echo "<h4>Other promises in bundle <a href=\"bundle.php?bundle=$mybundle\"><span id=\"bundle\">$mybundle</span></a></h4>";
$allhandles = cfpr_list_handles_for_bundle($mybundle,"agent",false); 
echo "$allhandles"; 

echo "<h4>Other promises by promiser</h4>";

$promiser = cfpr_get_promiser($handle);
$allhandles = cfpr_list_handles($promiser,"",false);
echo "$allhandles";

$type = cfpr_get_promise_type($handle);
echo "<h4>Other promises of type $type</h4>";
$allhandles = cfpr_list_handles("",$type,false);
echo "$allhandles"; 

echo "</td>";

echo "<td valign=\"top\">
  <h2>Promise definition (<a href=\"promise.php?handle=$handle&expand=1\">expand</a>): </h2>

<p>";

$promise = cfpr_summarize_promise($handle);

echo "$promise<p>";

if($expand == "1")
{
  echo "<h3>Expanded promise: </h3>";
  //$expandedPromise = cfpr_summarize_promise_exp($handle);
  //echo "$expandedPromise<p>";
}

if ($pid > 0) 
   {
#   $topic = cfpr_show_topic($pid);
#   echo "<p>$topic";
   $leads = cfpr_show_topic_leads($pid);    
   echo "<p>$leads"; 
   }

?>
</td>
</tr>
</table>
</div>

<?php
cfpr_footer();
?>
