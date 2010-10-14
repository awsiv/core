
<?php
cfpr_header("status","normal");

$hostkey = NULL;
$name = ".*";
$regex = 1;

//$ret = cfpr_report_classes(NULL,$name,true);
//$ret = cfpr_report_software_in(NULL,NULL,NULL,NULL,0);

$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;
?>
   <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php" class="current">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Status : bundles
             </span>
             <div class="clearleft"></div>
        </div>
         <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Services and methods</div>
                <div class="panelcontent">
                <div class="tables">
				<?php
                //$ret = cfpr_report_lastseen($hostkey,$hash,$host,$addr,$tago,$regex);
                $ret = cfpr_report_bundlesseen($hostkey,$name,$regex,NULL);
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



