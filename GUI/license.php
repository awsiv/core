<?php
cfpr_header("license usage status","normal");
$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();
$expiry = cfpr_getlicense_expiry();
$txt = cfpr_getlicense_summary();
?>
      <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Login:Max Manus
             </span>
             <div class="clearleft"></div>
        </div>
        
        <div id="tabpane">
        <div class="pagepanel">
         <div class="panelhead">License summary</div>
          <div class="panelcontent">
          <?php
		  echo "<ul><li>Granted $ret2 licenses. <li>Expiring on: $expiry</ul>";
          echo "<div class=\"tables\">$txt</div>";
		  ?>
          </div>
        </div>
        </div>
  <?php
include 'footer.php';
?>   