<?php
cfpr_header("service catalogue","normal");
cfpr_menu("Planning : services");
?>
        
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
cfpr_footer();
?>