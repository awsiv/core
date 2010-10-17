<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("license usage status","normal");
cfpr_menu("License : status");
$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();
$expiry = cfpr_getlicense_expiry();
$txt = cfpr_getlicense_summary();
?>
        
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
 cfpr_footer();
?>   