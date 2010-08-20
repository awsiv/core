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
	$name = $_POST['name'];
	$report = cfpr_report_bundlesseen(NULL,$name,true);
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$days = $_POST['days'];
	$months = $_POST['months'];
	$years = $_POST['years'];
	echo "<h4>Days: $days<br>Months: $months<br>Years: $years</h4>";
	$report = cfpr_report_value(NULL,$days,$months,$years);
	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$report = cfpr_report_classes(NULL,$name,true);
	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$state = $_POST['state'];
	$report = cfpr_report_compliance_promises(NULL,$name,$state,true);
	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">");
	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$cal = -1;
	$report = cfpr_report_filechanges(NULL,$name,true,-1,">");    
	break;
	
      case "File change diffs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$diff = $_POST['diff'];
	$cal = -1;
	$report = cfpr_report_filediffs(NULL,$name,$diff,true,$cal,">");
	break;
	
      case "Last saw hosts":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$address = $_POST['address'];
	$key = $_POST['key'];
	$ago = $_POST['ago'];
	if ($ago == 0)
	   {
	   $ago = -1;
	   }
	$report = cfpr_report_lastseen(NULL,$key,$name,$address,$ago,true);
	break;
	
      case "Patches available":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	$report = cfpr_report_patch_avail(NULL,$name,$version,$arch,true);
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	$report = cfpr_report_patch_in(NULL,$name,$version,$arch,true);
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$report = cfpr_report_performance(NULL,$name,true);
	break;

      case "Promises repaired summary":	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$report = cfpr_report_repaired(NULL,$name);
	break;
	
      case "Promises not kept summary":
      case "Promises not kept log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$report = cfpr_report_notkept(NULL,$name);
	break;
		
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$ret = cfpr_report_setuid(NULL,$name,true);
	break;
	
      case "Software installed":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	echo "<h4>Name: $name<br>Version: $version<br>Arch: $arch</h4>";
	$report = cfpr_report_software_in(NULL,$name,$version,$arch,true);
	break;
	
      case "Variables":
	echo "<h4>$report_type</h4>";
	$scope = $_POST['scope'];
	$lval = $_POST['lval'];
	$rval = $_POST['rval'];
	$type = $_POST['type'];
	$report = cfpr_report_vars(NULL,$scope,$lval,$rval,$type,true);
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
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Bundle pattern: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	break;
	
      case  "Business value report":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Date:<p><input class=\"searchfield\" type=\"text\" name=\"days\" size=\"80\">";
	echo "<p>Months:<p><input class=\"searchfield\" type=\"text\" name=\"months\" size=\"80\">";
	echo "<p>Years:<p><input class=\"searchfield\" type=\"text\" name=\"years\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	break;
	
      case "Class profile":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Class pattern: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	break; 
	
      case "Compliance by promise":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise by handle: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><select name=\"state\"><option value=\"x\">Any</option><option value=\"c\">Compliant</option><option value=\"r\">Repaired</option><option value=\"n\">Non-compliant</option></select>";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";

	break;
	
      case "File change log":
	echo "<h1>$report_type</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>File name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
        echo "<p>CALENDAR";
 	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "File change diffs":
	echo "<h1>$report_type</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>File name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>Match content: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"diff\" size=\"80\">";
        echo "<p>CALENDAR";
 	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Last saw hosts":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Host name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>Host address: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"address\" size=\"80\">";
	echo "<p>Host's unique key:<p><input class=\"searchfield\" type=\"text\" name=\"key\" size=\"80\">";
	echo "<p>Hours ago:<p><input class=\"searchfield\" type=\"text\" name=\"ago\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";


	break;
	
      case "Patches available":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>package version: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\">";
	echo "<p>Package architecture:<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Patch status":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>package version: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\">";
	echo "<p>Package architecture: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Job name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	
	break;
	
      case "Promises repaired log":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Promises not kept log":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Setuid/gid root programs":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Filename: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Software installed":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p>package version: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\">";
	echo "<p>Package architecture: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Variables":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Scope or bundle: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"scope\" size=\"80\">";
	echo "<p>Lvalue or name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"lval\" size=\"80\">";
	echo "<p>Rvalue or content: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"rval\" size=\"80\">";
	echo "<p>Type:<p>";
	echo "<select name=\"type\"><option value=\"s\">String</option><option value=\"sl\">String list</option><option value=\"i\">Integer</option><option value=\"il\">Integer list</option><option value=\"r\">Real</option><option value=\"rl\">Real list</option></select>";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
      }

   echo "</div>\n";
  }


cfpr_footer();
?>



