<?php
cfpr_header("results","ok");
#
# (C) Cfengine AS, PHP temporary solution
#


$search = $_POST['search'];
$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];

#$select = cfpr_select_hosts(".*",100);
#echo $select;

if ($search == "")
  {
  $search = ".*";
  }

switch ($report_type)
  {
  case  "Bundle profile":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_bundlesseen($hostkey,$search,true);
    break;
    
  case  "Business value report":
    echo "<h4>$report_type</h4>";
    break;
    
   case "Class profile":
    echo "<h4>$report_type</h4>";
     $report = cfpr_report_classes($hostkey,$search,true);
     break; 
 
  case "Compliance by promise":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_compliance_promises($hostkey,$search,"x",true);
    break;
    
  case "Compliance summary":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">");
    break;
    
  case "File change log":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_filechanges($hostkey,$search,true,-1,">");    
    break;
    
  case "File change diffs":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_filediffs($hostkey,$search,NULL,true,-1,">");
    break;

  case "Last saw hosts":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_lastseen($hostkey,NULL,$search,NULL,-1,true);
    break;

  case "Patches available":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_patch_avail($hostkey,$search,NULL,NULL,true);
    break;

  case "Patch status":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_patch_in($hostkey,$search,NULL,NULL,true);

    break;

  case "Performance":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_performance($hostkey,$search,true);
    break;

  case "Promises repaired":
    echo "<h4>$report_type</h4>";
    break;

  case "Promises not kept":
    echo "<h4>$report_type</h4>";
    break;

  case "Setuid/gid root programs":
    echo "<h4>$report_type</h4>";
    $ret = cfpr_report_setuid($hostkey,$search,true);
    break;
    
  case "Software installed":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_software_in($hostkey,$search,NULL,NULL,true);
    break;

  case "Variables":
    echo "<h4>$report_type</h4>";
    $report = cfpr_report_vars($hostkey,NULL,$search,NULL,NULL,true);
    break;
  }

if ($report)
  {
  echo $report;
  }
else
  {
  echo "<div id=\"selector\"><h2>No results</h2></div>";
  }


cfpr_footer();
?>



