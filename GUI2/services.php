<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

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
                
</script>
        
 <?php
cfpr_footer();
?>