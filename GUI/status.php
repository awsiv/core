  <?php 
    cfpr_header("engineering status","normal");  
	$ret1 = cfpr_getlicense_owner();
	$all = cfpr_count_all_hosts();
	$r = cfpr_count_red_hosts();
	$y = cfpr_count_yellow_hosts();
	$g = cfpr_count_green_hosts();

  cfpr_menu("Status : hosts");
  ?>       
        
        <div id="tabpane">
          <div class="grid_4">
          <div class="panel">
             <div class="panelhead"> Hosts</div>
                 <ul class="panelcontent">
                 <li>Host Known: <?php echo $all?></li>
                 <li>Ranked worst:<a href="topN.php">Show top 50</a></li>
                </ul>
           </div>
          
                  <div class="panel">
                    <div class="panelhead"> Status</div>
                     <ul class="panelcontent">
 <li><a href="hosts.php?type=red"><img src="images/red_sign_medium.png" class="align"/><span class="imglabel"><?php echo $r?> hosts known</span></a></li>
 <li><a href="hosts.php?type=yellow"><img src="images/yellow_sign_medium.png" class="align"/><span class="imglabel"><?php echo $y?> hosts known</span></a></li>
 <li><a href="hosts.php?type=green"><img src="images/green_sign_medium.png" class="align"/><span class="imglabel"><?php echo $g?> hosts known</span></a></li>

                     </ul>
                  </div>
           </div>
        <?php cfpr_compliance_summary_graph();?>  
          <div class="grid_8">
           	<div class="panel">
          		<div class="panelhead">Average meters for all hosts</div>
                <div class="panelcontent">
                    <ul>
                      <li> <a href="knowledge.php?topic=Status level meters"><img src="/hub/common/meter.png"></a></li>
                       <li><img class="icontext" src="/img/dive.png"> <a href="host.php">Select a particular host</a></li>
                     </ul>
                 </div>
          	</div>
            
            <div class="panel">
          		<div class="panelhead">Search groups of hosts</div>
                <div class="panelcontent">
                    <form method="post" action="search.php">
                          <p><?php $allreps = cfpr_select_reports(".*",100);
                          echo "$allreps";?>
                          <p>
                          <input type="submit" value="step 1 of 2">
                     </form>
                 </div>
          	</div>
            
            <div class="panel">
          		<div class="panelhead">Special purpose policies</div>
                <div class="panelcontent">
                    <form method="post" action="spp.php">
                          <p><?php $allSppReps = cfpr_spp_reportnames();
                          echo "$allSppReps";?>
                          <p>
                          <input type="submit" value="Generate">
                     </form>
                 </div>
          	</div>
            
          </div>
          
          <div class="clear"></div>
        </div>
        
  <?php
 cfpr_footer();
 ?>
