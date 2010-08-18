<?php
$search = $_POST['search'];
$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];
$many = $_POST['manyhosts'];

if ($report_type == "")
   {
   $search = $_GET['search'];
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }

$hostname =  cfpr_hostname($hostkey);

cfpr_header("search results","ok");

if ($search == "")
  {
  $search = ".*";
  }

if ($many)  // Returning query
  {
  switch ($report_type)
      {
      case  "Bundle profile":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_bundlesseen($hostkey,$search,true);
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_value($hostkey,NULL,NULL,NULL);
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
	$name = $_POST['name'];
	$name = $_POST['version'];
	$name = $_POST['arch'];
	$report = cfpr_report_patch_avail($hostkey,$name,$version,$arch,true);
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$name = $_POST['version'];
	$name = $_POST['arch'];
	$report = cfpr_report_patch_in($hostkey,$name,$version,$arch,true);
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_performance($hostkey,$search,true);
	break;
	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	break;
	
      case "Promises not kept log":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	break;
	
      case "Promises repaired summary":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	break;
	
      case "Promises not kept summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	break;
	
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$ret = cfpr_report_setuid($hostkey,$search,true);
	break;
	
      case "Software installed":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$name = $_POST['version'];
	$name = $_POST['arch'];
	$report = cfpr_report_software_in($hostkey,$name,$version,$arch,true);
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

  }
else if ($hostkey != "") // Default search on single machine
   {
   switch ($report_type)
      {
      case  "Bundle profile":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_bundlesseen($hostkey,$search,true);
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_value($hostkey,NULL,NULL,NULL);
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
	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	break;
	
      case "Promises not kept log":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	break;
	
      case "Promises repaired summary":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	break;
	
      case "Promises not kept summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
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
   }
else // No hosktkey
   {
   echo "<div id=\"advancedquery\">";

   switch ($report_type)
      {
      case  "Bundle profile":
	echo "<h4>$report_type</h4>";

	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";

	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";

	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";

	break;
	
      case "File change diffs":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Last saw hosts":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Patches available":
	echo "<h4>$report_type query</h4>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"200\">";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\" >";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\" >";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Patch status":
	echo "<h4>$report_type query</h4>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\" >";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\" >";
	echo "<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\" ";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Promises not kept log":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";

	break;
	
      case "Software installed":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name:<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>package version:<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\">";
	echo "<p>Package architecture:<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Variables":
	echo "<h4>$report_type</h4>";

	break;
      }

   echo "</div>\n";
  }


cfpr_footer();
?>



