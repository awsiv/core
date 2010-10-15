<?php
$handle = $_POST['handle'];

if ($hostkey == "")
   {
   $handle = $_GET['handle'];
   }

$title = "promise $handle";
cfpr_header("$title","normal");
cfpr_menu("Show : Promise");
$pid = cfpr_get_pid_for_topic("promises","$handle");
?>
     <div id="tabpane"> 
       <div class="pagepanel">
     <div class="grid_5">
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
    ?>
</div>
<div class="grid_7">
 <div class="panel">
  <div class="panelhead">Promise definition: </div>
  <div class="panelcontent">
   <div class="tables">
	<?php 
    $promise = cfpr_summarize_promise($handle);
    echo "$promise<p>";
    
    if ($pid > 0) 
       {
    #   $topic = cfpr_show_topic($pid);
    #   echo "<p>$topic";
       $leads = cfpr_show_topic_leads($pid);    
       echo "<p>$leads"; 
       }
    
    ?>
    </div>
       </div>
      </div>
     </div>
     <div class="clearleft"></div>
     </div>
  </div>
  <script type="text/javascript">
  $(document).ready(function() { 
			 $('#tabpane').find('h4').addClass('panelhead');
             $('#tabpane').find('ul').addClass('panelcontent');				 
	});
  </script>
  <?php
  cfpr_footer();
  ?>