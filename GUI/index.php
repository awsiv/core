<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("overview","normal");  
cfpr_menu("Home : overview");

$ret1 = cfpr_getlicense_owner();
$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();
  
?>       
        
        <div id="tabpane">
          <div class="grid_4">
                  <div class="panel">
                    <div class="panelhead"><?php echo $all?> hosts registerd</div>
                     <ul class="panelcontent">
<li><a href="hosts.php?type=red"><img src="images/red_sign_medium.png" class="align"/><span class="imglabel"><?php echo $r?> hosts known</span></a></li>
<li><a href="hosts.php?type=yellow"><img src="images/yellow_sign_medium.png" class="align"/><span class="imglabel"><?php echo $y?> hosts known</span></a></li>
 <li><a href="hosts.php?type=green"><img src="images/green_sign_medium.png" class="align"/><span class="imglabel"><?php echo $g?> hosts known</span></a></li>
     <li class="note"><a href="license.php"><img src="images/info.png" class="align"/><span class="imglabel">This edition is licenced to <?php echo $ret1?></span></a></li>
                     </ul>
                     <p>
                     
                     </p>
                  </div>
           </div>
        <?php cfpr_compliance_summary_graph();?>  
          <div class="grid_8">
           	<div class="panel">
          		<div class="panelhead">Compliance promise summary</div>
                <div class="panelcontent">
                  <img src="/hub/common/compliance.png" class="grid_6"/>
                  <ul class=" grid_2 grp_label">
                  	<li><img src="images/button-green.png" class="align"/><span class="imglabel">kept</span></li>
                    <li><img src="images/button-yellow.png" class="align"/><span class="imglabel">repaired</span></li>
                    <li><img src="images/button-red.png" class="align"/><span class="imglabel">not kept</span></li>
                    <li><img src="images/button-orange.png" class="align"/><span class="imglabel">no data</span></li> 
                  </ul>
                  <div class="clear"></div>
                 </div>
          	</div>
          </div>
          
          <div class="clear"></div>
        </div>
        
<?php
cfpr_footer();
?>