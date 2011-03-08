<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#
?>       
          <div class="grid_4">
         
          
                  <!--<div class="panel">
                    <div class="panelhead"> Status</div>
                     <ul class="panelcontent">
 <li><a href="<?php echo site_url('/welcome/hosts/red')?>"><img src="<?php echo get_imagedir();?>red_sign_medium.png" class="align"/><span class="imglabel"><?php echo $r?> hosts known</span></a></li>
 <li><a href="<?php echo site_url('/welcome/hosts/yellow')?>"><img src="<?php echo get_imagedir();?>/yellow_sign_medium.png" class="align"/><span class="imglabel"><?php echo $y?> hosts known</span></a></li>
 <li><a href="<?php echo site_url('/welcome/hosts/green')?>"><img src="<?php echo get_imagedir();?>/green_sign_medium.png" class="align"/><span class="imglabel"><?php echo $g?> hosts known</span></a></li>

                     </ul>
                  </div>-->

              <div class="panel">
                    <div class="panelhead"> View Reports </div>
                    <div id="reportcontrol" class="panelcontent">

                    </div>
               </div>
           <div class="panel">
             <div class="panelhead"> Hosts</div>
                 <ul class="panelcontent">
                 <li>Hosts Known: <?php echo $all?></li>
                 <li>Ranked worst: <a href="weakest_host">Show top 50</a></li>
                </ul>
           </div>
           </div>
          <div class="grid_8">
              <div id="repdialog" class="hidden"></div>
           	<div class="panel">
          		<div class="panelhead">Average meters summarizing all hosts</div>
                <div class="panelcontent">
                    <ul>
                      <li><?php include_once('graph/summaryCompliance.php'); ?></li>
                       <li><img class="icontext" src="<?php echo get_imagedir();?>/host.png"> <a href="<?php echo site_url('/welcome/host')?>">Select a particular host</a></li>
                     </ul>
                 </div>
          	</div>
            
            <!--<div class="panel">
              <div class="panelhead">Reporting: search groups of hosts</div>
                <div class="panelcontent">
                    <form method="post" action="<?php echo site_url('search/index');?>">
                        <p><?php echo form_dropdown('report',$allreps,'Bundle profile')?></p>
                          <input type="submit" value="step 1 of 2">
                     </form>
                 </div>
          	</div>-->
            
            <div class="panel">
          		<div class="panelhead">Summary of Content-Driven Policies</div>
                <div class="panelcontent">
                    <form method="post" action="<?php echo site_url('cdp/index')?>">
                          <p><?php echo $allSppReps;?><p>
                          <input class="btn" type="submit" value="Generate">
                     </form>
                 </div>
          	</div>
            
          </div>
          
          <div class="clear"></div>
          <script type="text/javascript">
            reportcontrol(<?php echo $jsondata?>,"overall");
          </script>

