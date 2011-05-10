<?php
				$last = cfpr_getlastupdate($hostkey);
				$class = cfpr_get_variable($hostkey,"sys","ostype");
				$flavour = cfpr_get_variable($hostkey,"sys","flavour");
				$rel = cfpr_get_variable($hostkey,"sys","release");
				$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
				$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
				$speed = cfpr_get_network_rate($hostkey);
			  ?>
<div class="outerdiv">
    <div class="innerdiv">
			<?php $report = cfpr_summarize_notkept($hostkey,NULL,NULL,NULL,NULL);?>
              <p class="title">Promises not kept in the past week</p>
                      <div class="tables">

                      <?php
                      $tableData = json_decode($report,true);
                      if (is_array($tableData)) {
                      echo  $this->cf_table->generateReportTable($tableData);
                      } else echo "No data found";
                      ?>
                      </div>
     </div>
    <div class="grid_4">
        <div id="hostviewerbtn" class="dash innerdiv">
            <p class="title">Actions</p>
            <ul>
                <li><?php echo anchor('widget/allreports','Reports',array('id'=>'findreport')) ?></li>
                    <?php
                     if ($is_commented != "")
                         {
                          $noteUrl = site_url('notes/index/action/show/nid').'/'.$is_commented.'/';
                         }
                         else {
                             $noteUrl = site_url('notes/index/action/add/hostkey').'/'.$hostkey.'/';

                         }
                      ?>
                <li><a href="<?php echo $noteUrl ?>" class="note" id="add_cmt">Notes</a></li>
                <li> <?php echo anchor('visual/vitals/'.$hostkey,'vitals',array('id'=>'pulseNvitals')) ?></li>
                <p class="clearleft"></p>
            </ul>
        </div>
      <div id="monitoredjob"class="innerdiv">
         <p class="title">Monitored jobs</p>
                <?php
                $number = cfpr_get_class_frequency(NULL,"mXC.*");
                $nlist = cfpr_report_class_frequency($hostkey,"mXC.*");
                ?>
                <p>Total number under surveillance:<?php echo $number?><p>
                <p>On this host: <?php echo $nlist?></p> <!--proble, with varaibale nlist--> 
       </div>

         <div id="statusmeasured" class="innerdiv ">
                   <p class="title">Status (measured)</p>
              
               <p><label class="width_20">Average Load:</label><label ><?php echo $load?>%</label></p>
               <p> <label class="width_20">Average Free Disk:</label><label><?php echo $free?>%</label></p>
               <p><label class="width_20">Average network speed:</label><label><?php echo  $speed?></label></p>
                    
           </div>
    </div>
    <div id="hostviewerrightpanes" class="grid_8">
            <div class="innerdiv">
                <p class="title">Analysis</p>
                <div id="hostviewgraph" class="graphcontainer"><?php include_once('graph/summaryCompliance.php'); ?></div>
                <!--<p><a href="<?php echo site_url('visual/vital').'/'.$hostkey ?>" ><img src="<?php echo get_imagedir()?>pulsed.png" class="align"/></a></p>-->
            </div>
            <div class="innerdiv">
                
                     <p class="title">Host Details (discovered)</p>
                       <p><label class="width_20">Alias:</label><label ><?php echo $hostname?></label></p>
                       <p><label class="width_20">OS class:</label><label><?php echo $class?></label></p>
                       <p><label class="width_20">Release:</label><label><?php echo $rel?></label></p>
                       <p><label class="width_20">Flavour:</label><label><?php echo $flavour?></label></p>
                       <p><label class="width_20">Last IP-address:</label><label><?php echo $ipaddr?></label></p>
                       <p><label class="width_20">Last data:</label><label><?php echo getDateStatus($last);?></label></p>
                       <p><label class="width_20">ID:</label><label><small><?php echo $hostkey?></small></label></p>
             </div>
    </div>
    <div class="clear"></div>
</div>
<script type="text/javascript">
$(document).ready(function() {
   // $('.tables table').tableFilter();
    //$('.tables table').tablesorter({widgets: ['zebra']});

    $('.note').ajaxyDialog({change:function(nid,element) {

                // change the url
               $(element).attr('href', '/notes/index/action/show/nid/' + nid);
               // console.log(nid,element);
        }});
    //reportcontrol(<?php echo $jsondata?>,"host");
    $('#findreport').reportfinder({allhost:false,hostkey:"<?php echo $hostkey;?>"});
});
</script>
