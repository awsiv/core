<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#
      
$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];
$op=$_POST['op'];

if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

if ($hostkey == "")
   {
   $hostkey = "none";
   }
if ($op=="delete")
{
	$host=$_POST['delhost'];
	cfpr_delete_host($host);
}

$hostname = cfpr_hostname($hostkey);
$ipaddr = cfpr_ipaddr($hostkey);
$title = "host $hostname";
cfpr_header("$title","normal");
cfpr_menu("Status : host");
cfpr_host_meter($hostkey);
$colour = cfpr_get_host_colour($hostkey);
  ?>
        
        <div id="tabpane">
         <div class="grid_5">
                  <div class="panel">
                    <div class="panelhead">Select host:</div>
                     <div class="panelcontent">
                     <form method="post" action="host.php">
                     <?php
                     $allhosts = cfpr_select_hosts($hostkey,".*",100);
                     echo "$allhosts";
					 ?>
                     <input class="btn floatRight"  type="submit"  value="Select host" />
                     <div class="clearright"></div>
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
                        <div class="panelhead">Host Details</div>
                     <div class="panelcontent">
       <p><label class="width_20">Alias:</label><label ><?php echo $hostname?></label></p>
      <p> <label class="width_20">OS Class:</label><label><?php echo $class?></label></p>
       <p><label class="width_20">Release:</label><label><?php echo $rel?></label></p>
        <p><label class="width_20">Flavour:</label><label><?php echo $flavour?></label></p>
       <p><label class="width_20">Last IP-address:</label><label><?php echo $ipaddr?></label></p>
       <p><label class="width_20">Last Update:</label><label><?php echo $last?></label></p>
       <p><label class="width_20">ID:</label><label><small><?php echo $hostkey?></small></label></p>
                      </div>
                 </div>
                 
                 <div class="panel">
                        <div class="panelhead">Status</div>
                     <div class="panelcontent">
               <div class="width_80 floatLeft">
               <p><label class="width_20">Average Load:</label><label ><?php echo $load?>%</label></p>
               <p> <label class="width_20">Average Free Disk:</label><label><?php echo $free?>%</label></p>
               <p><label class="width_20">Average network speed:</label><label><?php echo  $speed?></label></p>
               </div>
               <div class="floatRight">
               <?php echo "<img src=\"images/$colour"."_sign_medium.png\" class=\"floatRight\"/>"; ?>
                <form method="post" action="host.php" onsubmit="return confirm('Are you sure you want to delete this host?');">
                     <input type="hidden" name="op" id="op" value="delete"/>
                     <input type="hidden" name="delhost" id="delhost" value="<?php echo $hostkey?>"/>
                     <input class="btn margin-top10"  type="submit"  value="Delete host"/>
                 </form>
                </div>
               <div class="clearboth"></div>
                     </div>
                  </div>
             </div>
          <?php cfpr_summary_meter(NULL);?>
          <div class="grid_7">
           	<div class="panel">
          		<div class="panelhead">Analysis</div>
                <div class="panelcontent">
                <p><a href="knowledge.php?topic=Status level meters"><img src="/hub/<?php echo $hostkey?>/meter.png"></a></p>
         <p><a href="vitals.php?hostkey=<?php echo $hostkey?>"><img src="images/pulsed.png" class="align"/><span class="imglabel">Pulse and vital sign for this host</span></a></p>
                 </div>
          	</div>
            
            <div class="panel">
          		<div class="panelhead">Generate report</div>
                <div class="panelcontent">
                <form method="post" action="search.php">
                <p>Simple search string:</p>
                <p><input class="searchfield" type="text" name="search" /></p>
                <p><?php $allreps = cfpr_select_reports(".*",100);
				  echo "$allreps";
				?></p>
                <input type="hidden" name="hostkey" value="<?php echo $hostkey?>">
                <p><input type="submit" value="commit"></p>
                </form>
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
			<?php $report = cfpr_summarize_notkept($hostkey,NULL,NULL);?>
			<div class="panelhead">Promises not kept in the past week</div>
                <div class="panelcontent">
                      <div class="tables">
                      <?php echo $report ?>
                      </div>
                </div>
              </div>
         </div>  
<script type="text/javascript">
$(document).ready(function() { 
    $('.tables table').prepend(
        $('<thead></thead>').append($('.tables tr:first').remove())
        );

    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']}); 
});
 
</script>
<?php
cfpr_footer();
?>
