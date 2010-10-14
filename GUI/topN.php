<?php
cfpr_header("weakest hosts","normal");
$ret = cfpr_topN_hosts("compliance",1000);
?>
<div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php" class="current">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Status : weak hosts
             </span>
             <div class="clearleft"></div>
    </div>
        <div id="tabpane">
            <div class="pagepanel">
             <div class="panelhead">Weakest host</div>
             <div class="panelcontent">
             <?php echo "<div class=\"tables\">$ret</div>" ?>
             </div>
            </div>
        </div>
<?php
include 'footer.php';
?> 