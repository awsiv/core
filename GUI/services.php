<?php
cfpr_header("service catalogue","normal");
?>
      <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php"  class="current">PLANNING</a></li>
              <li><a href="status.php">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Planning : services
             </span>
             <div class="clearleft"></div>
        </div>
        
        <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Services and methods</div>
                <div class="panelcontent">
                <div class="tables">
                <?php
                $services = cfpr_list_all_bundles(agent);  
                echo "$services";
                ?>
                </div>
                </div>
           </div>
        </div>
   <script type="text/javascript">
$(document).ready(function() { 
    $('.tables table').prepend(
        $('<thead></thead>').append($('.tables tr:first').remove())
        );

    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']}); 
});
 
</script>
        
 <?php
include 'footer.php';
?>