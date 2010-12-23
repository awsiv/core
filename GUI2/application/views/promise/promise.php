<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

?>
 <div class="pagepanel">
     <div class="grid_5">
<h4>Other promises in bundle <a href="<?php echo site_url('bundle/detail/bundle').$mybundle ?>"><span id=\"bundle\"><?php echo $mybundle?></span></a></h4>
<?php echo "$allhandles"?> 

<h4>Other promises by promiser</h4> 
<?php echo "$allhandlespromiser" ?>

<h4>Other promises of type <?php echo $type?></h4>
<?php echo "$allhandlesbytype"?> 

</div>
<div class="grid_7">
 <div class="panel">
  <div class="panelhead">Promise definition: </div>
  <div class="panelcontent">
   <div class="tables">

<?php 
echo "$promise<p>";

if ($pid > 0) 
   {
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
  <script type="text/javascript">
  $(document).ready(function() { 
			 $('#tabpane').find('h4').addClass('panelhead');
             $('#tabpane').find('ul').addClass('panelcontent');				 
	});
  </script>
