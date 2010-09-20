<?php

//$hostkey = $_POST['hostkey'];
$report_type = $_POST['spp_report'];

$hostkey = "";


cfpr_header("Special Purpose Policy Reports","ok");


echo "<br>\n<br>\n";

switch($report_type)
{
case "ACLs":
  echo "<h4>Access control lists (extended file permissions)</h4>\n";
  break;

case "Commands":
  echo "<h4>Commands</h4>\n";
  break;

case "File Changes":
  echo "<h4>File changes</h4>\n";
  break;

case "File Diffs":
  echo "<h4>File diffs</h4>\n";
  break;

case "Registry":
  echo "<h4>Registry</h4>\n";
  break;

case "Services":
  echo "<h4>Services</h4>\n";
  break;

default:
  echo "Unknown report type ($report_type)";

}

$reportTable = cfpr_spp_report(NULL,$report_type);
echo "$reportTable";


cfpr_footer();

?>

