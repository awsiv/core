<?php
$search = $_POST['search'];
$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];
$many = $_POST['manyhosts'];
$class_regex = $_POST['class_regex'];

if ($report_type == "")
   {
   $search = $_GET['search'];
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   $many = $_GET['manyhosts'];
   $class_regex = $_GET['class_regex'];
   }

$hostname =  cfpr_hostname($hostkey);

cfpr_header("search results","ok");

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
           $report = cfpr_hosts_with_bundlesseen(NULL,$name,true,$class_regex);
	   }
	else
	   {
	   $report = cfpr_report_bundlesseen(NULL,$name,true,$class_regex);
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";  
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\">Email PDF</a></div><br><br>";
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
	   $report = cfpr_hosts_with_value(NULL,$days,$months,$years,$class_regex);
	   }
	else
	   {
	   $report = cfpr_report_value(NULL,$days,$months,$years,$class_regex);
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&days=$days&months=$months&years=$years&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&days=$days&months=$months&years=$years&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	   }
	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	   {
	   $report = cfpr_hosts_with_classes(NULL,$name,true,$class_regex);
	   }
	else
	   {
	   $report = cfpr_report_classes(NULL,$name,true,$class_regex);
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
	   }
	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$state = $_POST['state'];

	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_compliance_promises(NULL,$name,$state,true,$class_regex);
	  }
	else
	  {
	    $report = cfpr_report_compliance_promises(NULL,$name,$state,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&state=$state&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";     
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&state=$state&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";     
	  }
	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
	  }
	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$cal = -1;
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_filechanges(NULL,$name,true,-1,">",$class_regex);    
	  }
	else
	  {	    
	    $report = cfpr_report_filechanges(NULL,$name,true,-1,">",$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	  }
	break;
	
      case "File change diffs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$diff = $_POST['diff'];
	$cal = -1;
	if ($hosts_only)
	   {
	   $report = cfpr_hosts_with_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
	   }
	else
	   {
	   $report = cfpr_report_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&diff=$diff&cal=$cal&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	   echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&diff=$diff&cal=$cal&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
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
	    $report = cfpr_hosts_with_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&address=$address&key=$key&ago=$ago&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&address=$address&key=$key&ago=$ago&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	  }
	break;
	
      case "Patches available":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	  }
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];

	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_patch_in(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {	  
	    $report = cfpr_report_patch_in(NULL,$name,$version,$arch,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
	  }
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_performance(NULL,$name,true,$class_regex);
	  }
	else
	  {	   
	    $report = cfpr_report_performance(NULL,$name,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	  }
	break;

      case "Promises repaired summary":	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_repaired(NULL,$name,$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_repaired(NULL,$name,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
	  }
	break;
	
      case "Promises not kept summary":
      case "Promises not kept log":
	echo "<h4 class=\"reportnotkept\">$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $report = cfpr_hosts_with_notkept(NULL,$name,$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_notkept(NULL,$name,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
	  }
	break;
		
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$name = $_POST['name'];
	if ($hosts_only)
	  {
	    $ret = cfpr_hosts_with_setuid(NULL,$name,true,$class_regex);
	  }
	else
	  {	    
	    $ret = cfpr_report_setuid(NULL,$name,true,$class_regex);
	    echo "<br><div id=\"banner&class_regex=$class_regex\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner&class_regex=$class_regex\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
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
	    $report = cfpr_hosts_with_software_in(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {	    
	    $report = cfpr_report_software_in(NULL,$name,$version,$arch,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";   
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";   
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
	    $report = cfpr_hosts_with_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
	  }
	else
	  {    
	    $report = cfpr_report_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&scope=$scope&lval=$lval&rval=$rval&var_type=$type&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	    echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&scope=$scope&lval=$lval&rval=$rval&var_type=$type&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
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
	$report = cfpr_report_bundlesseen($hostkey,$search,true,NULL,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case  "Business value report":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_value($hostkey,NULL,NULL,NULL,$class_regex);	
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&days=&months=&years=&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&days=&months=&years=&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Class profile":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_classes($hostkey,$search,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break; 
	
      case "Compliance by promise":
	echo "<h4>$report_type</h4>";
	$state = "x";
	$report = cfpr_report_compliance_promises($hostkey,$search,$state,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&state=$state&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&state=$state&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Compliance summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "File change log":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_filechanges($hostkey,$search,true,-1,">",$class_regex);    
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
       case "File change diffs":
	echo "<h4>$report_type</h4>";
	$cal = -1;
	$report = cfpr_report_filediffs($hostkey,$search,NULL,true,$cal,">",$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&cal=$cal&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&cal=$cal&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Last saw hosts":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_lastseen($hostkey,NULL,$search,NULL,-1,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&ago=-1&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&ago=-1&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Patches available":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_patch_avail($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Patch status":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_patch_in($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Performance":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_performance($hostkey,$search,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Promises repaired log":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL,$class_regex);
	$report = cfpr_report_repaired($hostkey,NULL,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Promises not kept log":
	echo "<h4 class=\"reportnotkept\">$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL,$class_regex);
	//$report = cfpr_summarize_notkept($hostkey,NULL,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Promises repaired summary":
	echo "<h4>$report_type</h4>";
	//$report = cfpr_summarize_repaired($hostkey,NULL,$class_regex);
	$report = cfpr_report_repaired($hostkey,NULL,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Promises not kept summary":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_notkept($hostkey,NULL,$class_regex);
	//$report = cfpr_summarize_notkept($hostkey,NULL,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Setuid/gid root programs":
	echo "<h4>$report_type</h4>";
	$ret = cfpr_report_setuid($hostkey,$search,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Software installed":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_software_in($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
	break;
	
      case "Variables":
	echo "<h4>$report_type</h4>";
	$report = cfpr_report_vars($hostkey,NULL,$search,NULL,NULL,true,$class_regex);
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\"></a></div><br><br>";
	echo "<br><div id=\"banner\"><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&pdf_action=email&class_regex=$class_regex\">Email PDF</a></div><br><br>";
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
	echo "<p>Host group: (.*+[])<p><input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\">";
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



