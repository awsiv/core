<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#

$colour = cfpr_get_host_colour($hostkey);
  ?>
        
         <div class="grid_4">
                
             <div class="panel">
                    <div class="panelhead"> View Reports </div>
                    <div id="reportcontrol" class="panelcontent">
                     <form method="post" action="<?php echo site_url('search/')?>">
                          <input type="hidden" name="hostkey" value="<?php echo $hostkey?>">
                    </form>
                    </div>
               </div>
              <?php
				$last = cfpr_getlastupdate($hostkey);
				$class = cfpr_get_variable($hostkey,"sys","ostype");
				$flavour = cfpr_get_variable($hostkey,"sys","flavour");
				$rel = cfpr_get_variable($hostkey,"sys","release");
				$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
				$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
				$speed = cfpr_get_network_rate($hostkey);
			  ?>
                  

                 <div class="panel">
                     <div class="panelhead">Comments</div>
                     <div class="panelcontent">
                     <p><label class="width_20">host Key:</label><label ><?php echo $hostname?></label></p>
                      <?php
                     if ($is_commented != "")
                         {
                          $noteUrl = site_url('notes/index/action/show/nid').'/'.$is_commented.'/';
                         }
                         else {
                             $noteUrl = site_url('notes/index/action/add/hostkey').'/'.$hostkey.'/';

                         }
                      ?>
                     <a href="<?php echo $noteUrl ?>" class="btn floatRight note" id="add_cmt">Notes</a>
                     
                      <div class="clearright"></div>
                     <div id="comments">
                     </div>

                     <div id="add_comment">
                       
                     </div>

		     </div>
		   </div>

                 <div class="panel">
                   <div class="panelhead">Status (measured)</div>
                     <div class="panelcontent">
               <div class="width_80 floatLeft">
               <p><label class="width_20">Average Load:</label><label ><?php echo $load?>%</label></p>
               <p> <label class="width_20">Average Free Disk:</label><label><?php echo $free?>%</label></p>
               <p><label class="width_20">Average network speed:</label><label><?php echo  $speed?></label></p>
               </div>
               <div class="floatRight">
               <img src="<?php echo get_imagedir().$colour."_sign_medium.png" ?>" class="floatRight"/>
                <form method="post" action="host.php" onsubmit="return confirm('The host will be deleted from the report database of the hub. The hub will still try to pull it for reports, and it may thus reappear. Are you sure you want to delete this host?');">
                     <input type="hidden" name="op" id="op" value="delete"/>
                     <input type="hidden" name="delhost" id="delhost" value="<?php echo $hostkey?>"/>
                     <input class="btn margin-top10"  type="submit"  value="Delete host"/>
                 </form>
                </div>
               <div class="clearboth"></div>
                     </div>
                  </div>
             </div>
          <div class="grid_8">
           	<div class="panel">
          		<div class="panelhead">Analysis</div>
                <div class="panelcontent">
                <p><?php include_once('graph/summaryCompliance.php'); ?></p>
                <p><a href="<?php echo site_url('visual/vital').'/'.$hostkey ?>" ><img src="<?php echo get_imagedir()?>pulsed.png" class="align"/></a></p>
                 </div>
          	</div>
         
                 <div class="panel">
                     <div class="panelhead">Host Details (discovered)</div>
                     <div class="panelcontent">
       <p><label class="width_20">Alias:</label><label ><?php echo $hostname?></label></p>
       <p><label class="width_20">OS class:</label><label><?php echo $class?></label></p>
       <p><label class="width_20">Release:</label><label><?php echo $rel?></label></p>
       <p><label class="width_20">Flavour:</label><label><?php echo $flavour?></label></p>
       <p><label class="width_20">Last IP-address:</label><label><?php echo $ipaddr?></label></p>
       <p><label class="width_20">Last data:</label><label><?php echo getDateStatus($last);?></label></p>
       <p><label class="width_20">ID:</label><label><small><?php echo $hostkey?></small></label></p>
                      </div>
                 </div>
             
              <div class="panel">
          		<div class="panelhead">Monitored jobs</div>
                <div class="panelcontent">
                <?php
                $number = cfpr_get_class_frequency(NULL,"mXC.*");
                $nlist = cfpr_report_class_frequency($hostkey,"mXC.*");
                ?>
                <p>Total number under surveillance:<?php echo $number?><p>
                <p>On this host: <?php echo $nlist?></p> <!--proble, with varaibale nlist-->
                </div>
              </div>
          </div>
          <div class="clear"></div>
          
          
			<div class="panel">
			<?php $report = cfpr_summarize_notkept($hostkey,NULL,NULL,NULL,NULL);?>
			<div class="panelhead">Promises not kept in the past week</div>
                <div class="panelcontent">
                      <div class="tables">

                      <?php
                      $tableData = json_decode($report,true);
                      if (is_array($tableData)) {
                      echo  $this->cf_table->generateReportTable($tableData);
                      } else echo "No data found";
                      ?>

                      </div>
                </div>
            </div>
 
<script type="text/javascript">
$(document).ready(function() { 
    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']});

    $('.note').ajaxyDialog({change:function(nid,element) {

                // change the url
               $(element).attr('href', '/notes/index/action/show/nid/' + nid);
               // console.log(nid,element);
        }});
	
    reportcontrol(<?php echo $jsondata?>,"host");
});
</script>