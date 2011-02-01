<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("Content-Driven Policy Reports","ok");
$report_type = $_POST['cdp_report'];
$hostkey = "";

cfpr_menu("Status : hosts");
?>


<?php
$reportheading="";

switch($report_type)
   {
   case "ACLs":
       $reportheading = "Access control lists (cdp_inputs/acl_directory_list.txt and cdp_inputs/acl_file_list.txt)";
       break;
       
   case "Commands":
       $reportheading = "Commands (cdp_inputs/command_list.txt)";
       break;
       
   case "File Changes":
       $reportheading = "File changes (cdp_inputs/file_change_list.txt)";
       break;
       
   case "File Diffs":
       $reportheading = "File diffs (cdp_inputs/file_diff_list.txt)";
       break;
       
   case "Registry":
       $reportheading = "Registry (cdp_inputs/registry_list.txt)";
       break;
       
   case "Services":
       $reportheading =  "Services (cdp_inputs/service_list.txt)";
       break;
       
   default:
       $reportheading = " Unknown report type ($report_type) ";
       
   }
$reportTable = cfpr_cdp_report(NULL,$report_type);
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
cfpr_footer();
?>
