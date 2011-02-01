<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("weakest hosts","normal");
$ret = cfpr_top_n_hosts("compliance",1000);
cfpr_menu("Status : weakest hosts");
?>
        <div id="tabpane">
            <div class="pagepanel">
             <div class="panelhead">Weakest host</div>
             <div class="panelcontent">
             <?php echo "<div class=\"tables\">$ret</div>" ?>
             </div>
            </div>
        </div>
<?php
cfpr_footer();
?> 
