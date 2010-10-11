<?php
include 'header.php';
$report_type = $_POST['spp_report'];
$hostkey = "";
?>
      <div id="nav">
             <ul class="grid_10">
                  <li><a href="index.php">SUMMARY</a></li>
                  <li><a href="helm.php">PLANNING</a></li>
                  <li><a href="status.php" class="current">STATUS</a></li>
                  <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Planning : services
             </span>
             <div class="clearleft"></div>
        </div>
        <?php
		     $reportheading="";
			switch($report_type)
			{
			case "ACLs":
			  $reportheading = "Access control lists (spp_inputs/acl_directory_list.txt and spp_inputs/acl_file_list.txt)";
			  break;
			
			case "Commands":
			 $reportheading = "Commands (spp_inputs/command_list.txt)";
			  break;
			
			case "File Changes":
			  $reportheading = "File changes (spp_inputs/file_change_list.txt)";
			  break;
			
			case "File Diffs":
			  $reportheading = "File diffs (spp_inputs/file_diff_list.txt)";
			  break;
			
			case "Registry":
			 $reportheading = "Registry (spp_inputs/registry_list.txt)";
			  break;
			
			case "Services":
			  $reportheading =  "Services (spp_inputs/service_list.txt)";
			  break;
			
			default:
			 $reportheading = " Unknown report type ($report_type) ";
			
			}
			$reportTable = cfpr_spp_report(NULL,$report_type);
		?>
        <div id="tabpane">
        <div class="pagepanel">
          		<div class="panelhead"><?php echo $reportheading;?></div>
                <div class="panelcontent">
                    <div class="tables">
                     <?php echo $reportTable;?>
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