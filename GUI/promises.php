<?php
cfpr_header("compliance","normal");

$hostkey = NULL;
$name = ".*";
$regex = 1;

//$ret = cfpr_report_classes(NULL,$name,true);
//$ret = cfpr_report_software_in(NULL,NULL,NULL,NULL,0);
//$ret = cfpr_report_lastseen($hostkey,$hash,$host,$addr,$tago,$regex);
$hash = NULL;
$host = NULL;
$addr = NULL;
$handle = NULL;
$tago = 0;
$status = NULL;
?>
 <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php"  class="current">PLANNING</a></li>
              <li><a href="status.php">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Planning : services
             </span>
             <div class="clearleft"></div>
        </div>
        
        <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Promises</div>
                <div class="panelcontent">
                <div class="tables">
				<?php
                $ret = cfpr_report_compliance_promises($hostkey,$handle,"x",false,NULL);
                echo "$ret";
                ?>
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
include 'footer.php';
?>
