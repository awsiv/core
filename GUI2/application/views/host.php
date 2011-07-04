<?php
				$last = cfpr_getlastupdate($hostkey);
				$class = cfpr_get_variable($hostkey,"sys","ostype");
				$flavour = cfpr_get_variable($hostkey,"sys","flavour");
				$rel = cfpr_get_variable($hostkey,"sys","release");
				$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
				$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
				$speed = cfpr_get_network_rate($hostkey);
			  ?>
<div id="hostview">
    <div class="outerdiv">
   <div class="innerdiv" id="notkepttable" title="Promises not kept in the past week">
			<?php $report = cfpr_summarize_notkept($hostkey,NULL,NULL,NULL,NULL,NULL,NULL);?>
              <!--<p class="title">Promises not kept in the past week</p-->
                     <div class="tables">
                      <?php
                      $tableData = json_decode($report,true);
                      if (is_array($tableData)) {
                      echo  $this->cf_table->generateReportTable($tableData);
                      } else
                      {
                          echo "<span class=\"nodata\">No Data Found</span>";
                      }
                      ?>
                      </div>
     </div>
    <div class="grid_4">
        <div id="hostviewerbtn" class="dash innerdiv">
            <p class="title">Actions</p>
            <ul>
                <li><?php echo anchor('widget/allreports','Reports',array('id'=>'findreport', 'class'=>'report')) ?></li>
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
                <li> <?php echo anchor('visual/vital/'.$hostkey,'vitals',array('id'=>'pulseNvitals','class'=>'pulseNvitals')) ?></li>
                <li><a href="#" id="notkeptlnk" title="Promises not kept in the past week" class="problems">Problems</a></li>
                <p class="clearleft"></p>
            </ul>
        </div>
      <div id="monitoredjob"class="innerdiv">
         <p class="title">Monitored jobs</p>
                <?php
                $number = cfpr_get_class_frequency(NULL,"mXC.*");
                $count=0;
                $nlist =json_decode(cfpr_report_class_frequency($hostkey,"mXC.*"),true) ;
               
              //  echo cfpr_report_class_frequency($hostkey,"mXC.*");
                ?>
                <p>Total number under surveillance:<?php echo $number?><p>
                <p>On this host: <?php
                foreach($nlist as $list)
                {
                   if($list['count']>0)
                   {
                   echo "<p>".$list['name'].":".$list['count']."</p>";
                   }
                   else
                   {
                       echo $count;
                       break;
                   }
                }?></p> <!--proble, with varaibale nlist-->
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
            <div id="hostinformation" class="innerdiv">
                
               
                    <p class="title">Host Details (discovered)</p>
                     <form  id="delform"method="post" action="/welcome/host">
                     <input type="hidden" name="delhost" id="delhost" value="<?php echo $hostkey?>"/>
                     <input class="btn"  type="submit" id="btnsubmit"  value="Delete this host"/>
                    </form>
                    <br class="clear">
                       <p><label>Alias: </label><label ><?php echo $hostname?></label></p>
                       <p><label>OS class: </label><label><?php echo $class?></label></p>
                       <p><label>Release: </label><label><?php echo $rel?></label></p>
                       <p><label>Flavour: </label><label><?php echo $flavour?></label></p>
                       <p><label>Last IP-address: </label><label><?php echo $ipaddr?></label></p>
                       <p><label>Last data: </label><label><?php echo getDateStatus($last);?></label></p>
                       <p><label>ID: </label><label><small><?php echo $hostkey?></small></label></p>
             </div>
    </div>
    <div class="clear"></div>
    </div>
</div>
<div id="hostdelconfirmation" title="Proceed Host Deletion">
    <span>
        The host will be deleted from the report database of the hub.
        The hub will still try to pull it for reports, and it may thus <strong>Reappear</strong>.Are you sure you want to delete this host?
    </span>
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
    
    $('#findreport').reportfinder({allhost:false,hostkey:"<?php echo $hostkey;?>"});

     var $notkeptbox = $('#notkepttable').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
                 width: 'auto',
		 buttons: {
		 'Ok': function() {
	         $(this).dialog('close');
	         }
		 },
		 open: function() {
		 //$(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus()
		 }
	 });
         
         $('#notkeptlnk').bind('click',function(event){
             event.preventDefault();
             $notkeptbox.dialog('open');
         });

         var $hostdelconfirm = $('#hostdelconfirmation').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
                 width: 400,
                 resizable:false,
		 buttons: {
		 'Yes': function() {
	         $(this).dialog('close');
                 $("#delform").submit();
	         },
                'NO': function() {
	         $(this).dialog('close');
	         }
		 },
		 open: function() {
		 //$(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus()

		 }
	 });

    /*$("#delform").submit(function(event){
        event.preventDefault();
        $hostdelconfirm.dialog('open');
    });*/

    $("[type=submit]").bind("click", function(e){
      e.preventDefault();
     $hostdelconfirm.dialog('open');
    });
});
</script>
