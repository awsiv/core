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
   $many = $_GET['manyhosts'];
   }

$hostname =  cfpr_hostname($hostkey);

cfpr_header("search results","ok");
//if($hostkey != "" )//&& $report_type == "Promises not kept log")
//{
// echo "<br><div id=\"banner\"><a href=\"/pdf_report/index.php\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
//}
if ($search == "")
  {
  $search = ".*";
  }

if ($many)  // Returning query
   {
   $hosts_only = $_POST['hosts_only'];

   switch ($report_type)
      {
      case  "Bundle profile":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
  	   {
           $report = cfpr_hosts_with_bundlesseen(NULL,$name,true);
	   }
	else
	   {
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";  
	   $report = cfpr_report_bundlesseen(NULL,$name,true);
	   }
	
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$days = $_POST['days'];
	$months = $_POST['months'];
	$years = $_POST['years'];
	echo "<h4>Days: $days<br>Months: $months<br>Years: $years</h4>";
	if ($hosts_only)
	   {
	   $report = cfpr_hosts_with_value(NULL,$days,$months,$years);
	   }
	else
	   {
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&days=$days&months=$months&years=$years\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	   $report = cfpr_report_value(NULL,$days,$months,$years);
	   }
	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	   {
	   $report = cfpr_hosts_with_classes(NULL,$name,true);
	   }
	else
	   {
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	   $report = cfpr_report_classes(NULL,$name,true);
	   }
	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$state = $_POST['state'];

	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_compliance_promises(NULL,$name,$state,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&state=$state\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";     
	    $report = cfpr_report_compliance_promises(NULL,$name,$state,true);
	  }
	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">");
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">");
	  }
	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$cal = -1;
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_filechanges(NULL,$name,true,-1,">");    
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    $report = cfpr_report_filechanges(NULL,$name,true,-1,">");    
	  }
	break;
	
      case "File change diffs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$diff = $_POST['diff'];
	$cal = -1;
	if ($hosts_only)
	   {
	   $report = cfpr_hosts_with_filediffs(NULL,$name,$diff,true,$cal,">");
	   }
	else
	   {
	   // TODO: generate pdf report
	   $report = cfpr_report_filediffs(NULL,$name,$diff,true,$cal,">");
	   }
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
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_lastseen(NULL,$key,$name,$address,$ago,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&address=$address&key=$key&ago=$ago\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_lastseen(NULL,$key,$name,$address,$ago,true);
	  }
	break;
	
      case "Patches available":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_patch_avail(NULL,$name,$version,$arch,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    $report = cfpr_report_patch_avail(NULL,$name,$version,$arch,true);
	  }
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];

	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_patch_in(NULL,$name,$version,$arch,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_patch_in(NULL,$name,$version,$arch,true);
	  }
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_performance(NULL,$name,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_performance(NULL,$name,true);
	  }
	break;

      case "Promises repaired summary":	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_repaired(NULL,$name);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_repaired(NULL,$name);
	  }
	break;
	
      case "Promises not kept summary":
      case "Promises not kept log":
	echo "<h4 class=\"reportnotkept\">$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_notkept(NULL,$name);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_notkept(NULL,$name);
	  }
	break;
		
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $ret = cfpr_hosts_with_setuid(NULL,$name,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    $ret = cfpr_report_setuid(NULL,$name,true);
	  }
	break;
	
      case "Software installed":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	echo "<h4>Name: $name<br>Version: $version<br>Arch: $arch</h4>";
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_software_in(NULL,$name,$version,$arch,true);
	  }
	else
	  {
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    $report = cfpr_report_software_in(NULL,$name,$version,$arch,true);
	  }
	break;
	
      case "Variables":
	echo "<h4>$report_type</h4>";
	$scope = $_POST['scope'];
	$lval = $_POST['lval'];
	$rval = $_POST['rval'];
	$type = $_POST['type'];

	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_vars(NULL,$scope,$lval,$rval,$type,true);
	  }
	else
	  {
	    //TODO: generate pdf report
	    $report = cfpr_report_vars(NULL,$scope,$lval,$rval,$type,true);
	  }
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
	 echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_value($hostkey,NULL,NULL,NULL);
	 echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&days=&months=&years=\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_classes($hostkey,$search,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";
	$state = "x";
	$report = cfpr_report_compliance_promises($hostkey,$search,$state,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&state=$state\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">");
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_filechanges($hostkey,$search,true,-1,">");    
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "File change diffs":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_filediffs($hostkey,$search,NULL,true,-1,">");
	break;
	
      case "Last saw hosts":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_lastseen($hostkey,NULL,$search,NULL,-1,true);
	 echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&ago=-1\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Patches available":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_patch_avail($hostkey,$search,NULL,NULL,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_patch_in($hostkey,$search,NULL,NULL,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_performance($hostkey,$search,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Promises not kept log":
	echo "<h4 class=\"reportnotkept\">$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Promises repaired summary":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Promises not kept summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$ret = cfpr_report_setuid($hostkey,$search,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	break;
	
      case "Software installed":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_software_in($hostkey,$search,NULL,NULL,true);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
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

   echo "<table><tr><td>";

   switch ($report_type)
      {
      case  "Bundle profile":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Bundle pattern: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	break;
	
      case "Class profile":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Class pattern: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Compliance summary":
	echo "<h4>$report_type (Not possible to search)</h4>";
	break;
	
      case "File change log":
	echo "<h1>$report_type</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>File name: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
        echo "<p>CALENDAR";
 	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";
	
	break;
	
      case "Promises repaired log":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Promises not kept log":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
	
      case "Setuid/gid root programs":
	echo "<h1>$report_type query</h1>";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Filename: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"name\" size=\"80\">";
	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
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
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\">";
	echo "<p><input type=\"submit\" value=\"Generate report\">";
	echo "</form>";

	break;
      }

   echo "</td><td><h2>Tips</h2><ul><li>Use regular expressions that match the <i>whole string</i> you want to find<li><a href=\"knowledge.php?topic=regular expression\">Learn about regular expressions</a></ul></td></tr></table>";
   echo "</div>\n";
  }


cfpr_footer();
?>



