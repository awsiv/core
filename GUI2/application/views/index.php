<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#

//cfpr_header("overview","normal");  
//cfpr_menu("Home : overview");
$ret1 = cfpr_getlicense_owner();
$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts(); 
?> 
          <div class="grid_4">
                  <div class="panel">
                    <div class="panelhead"><?php echo $all?> hosts registered</div>
                     <ul class="panelcontent">
<li><a href="<?php echo site_url()?>/welcome/hosts/red"><img src="<?php echo get_imagedir();?>red_sign_medium.png" class="align"/><span class="imglabel"><?php echo $r?> hosts known</span></a></li>
<li><a href="<?php echo site_url()?>/welcome/hosts/yellow"><img src="<?php echo get_imagedir();?>yellow_sign_medium.png" class="align"/><span class="imglabel"><?php echo $y?> hosts known</span></a></li>
 <li><a href="<?php echo site_url()?>/welcome/hosts/green"><img src="<?php echo get_imagedir();?>green_sign_medium.png" class="align"/><span class="imglabel"><?php echo $g?> hosts known</span></a></li>
     <li class="note"><a href="license"><img src="<?php echo get_imagedir();?>info.png" class="align"/><span class="imglabel">This edition is licenced to <?php echo $ret1?></span></a></li>
                     </ul>
                     <p>
                     
                     </p>
                  </div>
           </div>
        <?php cfpr_compliance_summary_graph();?>  
          <div class="grid_8">
           	<div class="panel">
          		<div class="panelhead">Compliance summary</div>
                <div class="panelcontent">
                  <img src="<?php echo get_hubdir();?>common/compliance.png" class="grid_6"/>
                  <ul class=" grid_2 grp_label">
                  	<li><img src="<?php echo get_imagedir();?>button-green.png" class="align"/><span class="imglabel">kept</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-yellow.png" class="align"/><span class="imglabel">repaired</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-red.png" class="align"/><span class="imglabel">not kept</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-orange.png" class="align"/><span class="imglabel">no data</span></li> 
                  </ul>
                  <div class="clear"></div>
                 </div>
          	</div>
          </div>
          
          <div class="clear"></div>

