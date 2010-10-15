<?php
cfpr_header("weakest hosts","normal");
$ret = cfpr_topN_hosts("compliance",1000);
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