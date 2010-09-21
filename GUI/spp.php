<?php

//$hostkey = $_POST['hostkey'];
$report_type = $_POST['spp_report'];

$hostkey = "";


cfpr_header("Special Purpose Policy Reports","ok");


echo "<br>\n<br>\n";

switch($report_type)
{
case "ACLs":
  echo "<h4>Access control lists (spp_inputs/acl_directory_list.txt and spp_inputs/acl_file_list.txt)</h4>\n";
  break;

case "Commands":
  echo "<h4>Commands (spp_inputs/command_list.txt)</h4>\n";
  break;

case "File Changes":
  echo "<h4>File changes (spp_inputs/file_change_list.txt)</h4>\n";
  break;

case "File Diffs":
  echo "<h4>File diffs (spp_inputs/file_diff_list.txt)</h4>\n";
  break;

case "Registry":
  echo "<h4>Registry (spp_inputs/registry_list.txt)</h4>\n";
  break;

case "Services":
  echo "<h4>Services (spp_inputs/service_list.txt)</h4>\n";
  break;

default:
  echo "Unknown report type ($report_type)";

}

$reportTable = cfpr_spp_report(NULL,$report_type);
echo "$reportTable";


cfpr_footer();

?>

