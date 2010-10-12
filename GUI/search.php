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

include 'header.php';
?>

     <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php" class="current">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Login:Max Manus
             </span>
             <div class="clearleft"></div>
        </div>
        <div id="tabpane">
        
<?php
//cfpr_header("search results","ok");
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
	$name = $_POST['name'];
	if ($hosts_only)
  	   {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
           $report = cfpr_hosts_with_bundlesseen(NULL,$name,true,$class_regex);
	   }
	else
	   {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";     
	  
	   $report = cfpr_report_bundlesseen(NULL,$name,true,$class_regex);
	   }
	
	break;
	
      case  "Business value report":
	$days = $_POST['days'];
	$months = $_POST['months'];
	$years = $_POST['years'];
	if ($hosts_only)
	   {
	   echo "<div class=\"pagepanel\"><div class=\"panelhead\">Days: $days<br>Months: $months<br>Years: $years</div>";
	   $report = cfpr_hosts_with_value(NULL,$days,$months,$years,$class_regex);
	   }
	else
	   {
		echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\"> $report_type Days: $days  Months: $months  Years: $years</span><a href=\"./pdf_report/index.php?type=$report_type&days=$days&months=$months&years=$years&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&days=$days&months=$months&years=$years&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";  
	  
	   $report = cfpr_report_value(NULL,$days,$months,$years,$class_regex);
	   }
	break;
	
      case "Class profile":
	$name = $_POST['name'];
	if ($hosts_only)
	   {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";   
	   $report = cfpr_hosts_with_classes(NULL,$name,true,$class_regex);
	   }
	else
	   {
	   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	   <a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	   $report = cfpr_report_classes(NULL,$name,true,$class_regex);
	   }
	break; 
	
      case "Compliance by promise":
	
	$name = $_POST['name'];
	$state = $_POST['state'];

	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_compliance_promises(NULL,$name,$state,true,$class_regex);
	  }
	else
	  {
	    echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&state=$state&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&state=$state&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";     
	    $report = cfpr_report_compliance_promises(NULL,$name,$state,true,$class_regex);
	  }
	break;
	
      case "Compliance summary":
	
	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	  }
	else
	  {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";   
	    $report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	  }
	break;
	
      case "File change log":
	$name = $_POST['name'];
	$cal = -1;
	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_filechanges(NULL,$name,true,-1,">",$class_regex);    
	  }
	else
	  {
		 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
		 <a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	   
	    $report = cfpr_report_filechanges(NULL,$name,true,-1,">",$class_regex);    
	  }
	break;
	
      case "File change diffs":
	
	$name = $_POST['name'];
	$diff = $_POST['diff'];
	$cal = -1;
	if ($hosts_only)
	   {
	  echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	   $report = cfpr_hosts_with_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
	   }
	else
	   {
	 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&diff=$diff&cal=$cal&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&diff=$diff&cal=$cal&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	   
	   $report = cfpr_report_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
	   }
	break;
	
      case "Last saw hosts":
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
	   echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
	  }
	else
	  {
	 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&address=$address&key=$key&ago=$ago&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	 <a href=\"./pdf_report/index.php?type=$report_type&search=$name&address=$address&key=$key&ago=$ago&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	     
	    $report = cfpr_report_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
	  }
	break;
	
      case "Patches available":
	
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	if ($hosts_only)
	  {
		  echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	    $report = cfpr_report_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
	  }
	break;
	
      case "Patch status":
	
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];

	if ($hosts_only)
	  {
		 echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_patch_in(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {
		  echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$archclass_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	     
	    $report = cfpr_report_patch_in(NULL,$name,$version,$arch,true,$class_regex);
	  }
	break;
	
      case "Performance":
	$name = $_POST['name'];
	if ($hosts_only)
	  {
		  echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_performance(NULL,$name,true,$class_regex);
	  }
	else
	  {
		  echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	   
	    $report = cfpr_report_performance(NULL,$name,true,$class_regex);
	  }
	break;

      case "Promises repaired summary":	
      case "Promises repaired log":
	
	$name = $_POST['name'];
	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_repaired(NULL,$name,$class_regex);
	  }
	else
	  {
		  echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
		  <a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	    
	    $report = cfpr_report_repaired(NULL,$name,$class_regex);
	  }
	break;
	
      case "Promises not kept summary":
      case "Promises not kept log":
	
	$name = $_POST['name'];
	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_notkept(NULL,$name,$class_regex);
	  }
	else
	  {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
		   <a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	    $report = cfpr_report_notkept(NULL,$name,$class_regex);
	  }
	break;
		
      case "Setuid/gid root programs":
	$name = $_POST['name'];
	if ($hosts_only)
	  {
		 echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $ret = cfpr_hosts_with_setuid(NULL,$name,true,$class_regex);
	  }
	else
	  {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
		   <a href=\"./pdf_report/index.php?type=$report_type&search=$name&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	    $ret = cfpr_report_setuid(NULL,$name,true,$class_regex);
	  }
	break;
	
      case "Software installed":
	$name = $_POST['name'];
	$version = $_POST['version'];
	$arch = $_POST['arch'];
	
	if ($hosts_only)
	  {
		echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type for Name: $name  Version: $version  Arch: $arch</div>";
	    $report = cfpr_hosts_with_software_in(NULL,$name,$version,$arch,true,$class_regex);
	  }
	else
	  {
		   echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type for Name: $name Version: $version Arch: $arch</span><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><img src=\"/img/icon_pdf.png\" class=\"floatRight\"><a href=\"./pdf_report/index.php?type=$report_type&search=$name&version=$version&arch=$arch&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	      
	    $report = cfpr_report_software_in(NULL,$name,$version,$arch,true,$class_regex);
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
	   echo "<div class=\"pagepanel\"><div class=\"panelhead\">$report_type</div>";
	    $report = cfpr_hosts_with_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
	  }
	else
	  {
	    //TODO: generate pdf report
		echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&scope=$scope&lval=$lval&rval=$rval&var_type=$type&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&scope=$scope&lval=$lval&rval=$rval&var_type=$type&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	   
	    $report = cfpr_report_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
	  }
	break;
      }
 
    if ($report)
       {
	   echo "<div class=\"tables\">";
       echo $report;
	   echo "</div></div> </div>";
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
	$report = cfpr_report_bundlesseen($hostkey,$search,true,$class_regex);
	 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case  "Business value report":
	
	$report = cfpr_report_value($hostkey,NULL,NULL,NULL,$class_regex);
	 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&days=&months=&years=&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&days=&months=&years=&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Class profile":
	$report = cfpr_report_classes($hostkey,$search,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break; 
	
      case "Compliance by promise":
	$state = "x";
	$report = cfpr_report_compliance_promises($hostkey,$search,$state,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Compliance summary":
	
	$report = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "File change log":
	$report = cfpr_report_filechanges($hostkey,$search,true,-1,">",$class_regex);    
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a>
	<div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
    case "File change diffs":
	$cal = -1;
	$report = cfpr_report_filediffs($hostkey,$search,NULL,true,$cal,">",$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&cal=$cal&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&cal=$cal&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Last saw hosts":
	$report = cfpr_report_lastseen($hostkey,NULL,$search,NULL,-1,tru,$class_regexe);
	 echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&ago=-1&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&ago=-1&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
     case "Patches available":
	$report = cfpr_report_patch_avail($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Patch status":
	$report = cfpr_report_patch_in($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Performance":
	$report = cfpr_report_performance($hostkey,$search,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Promises repaired log":
	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NUL,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Promises not kept log":
	
	$report = cfpr_report_notkept($hostkey,NULL,$class_regex);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Promises repaired summary":

	//$report = cfpr_summarize_repaired($hostkey,NULL);
	$report = cfpr_report_repaired($hostkey,NULL,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	<a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Promises not kept summary":
	$report = cfpr_report_notkept($hostkey,NUL,$class_regexL);
	//$report = cfpr_summarize_notkept($hostkey,NULL);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	<a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Setuid/gid root programs":
	$ret = cfpr_report_setuid($hostkey,$search,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	<a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Software installed":
	$report = cfpr_report_software_in($hostkey,$search,NULL,NULL,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	<a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
	
      case "Variables":
	$report = cfpr_report_vars($hostkey,NULL,$search,NULL,NULL,true,$class_regex);
	echo "<div class=\"pagepanel\"><div class=\"panelhead withpdfbtn\"><span class=\"text\">$report_type</span><a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex\"><img src=\"/img/icon_pdf.png\" class=\"floatRight\"></a>
	<a href=\"./pdf_report/index.php?type=$report_type&hostkey=$hostkey&search=$search&class_regex=$class_regex&pdf_action=email\"><img src=\"images/emailsend.png\" class=\"floatRight lnsendmail\"></a><div class=\"clearboth\"></div></div><div class=\"panelcontent\">";
	break;
      }
 
    if ($report)
       {
	   echo "<div class=\"tables\">";
       echo $report;
	   echo "</div></div> </div>";
       }
    else
       {
       echo "<div id=\"selector\"><h2>No results</h2></div>";
       }
   }
else // No hosktkey
   {
  echo "<div class=\"grid_5\">";
   switch ($report_type)
      {
      case  "Bundle profile":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Bundle pattern: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\" ></p>";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "</form></div></div>";
	break;
	
      case  "Business value report":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Date:<input class=\"searchfield\" type=\"text\" name=\"days\" ></p>";
	echo "<p>Months:<input class=\"searchfield\" type=\"text\" name=\"months\" ></p>";
	echo "<p>Years:<input class=\"searchfield\" type=\"text\" name=\"years\" ></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";
	break;
	
      case "Class profile":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Class pattern: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";
	break; 
	
      case "Compliance by promise":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise by handle: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p><select name=\"state\"><option value=\"x\">Any</option><option value=\"c\">Compliant</option><option value=\"r\">Repaired</option><option value=\"n\">Non-compliant</option></select></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Compliance summary":
	echo "<h4>$report_type (Not possible to search)</h4>";
	break;
	
      case "File change log":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>File name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
    echo "<p>CALENDAR</p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
 	echo "<p><input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<p><input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "File change diffs":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>File name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Match content: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"diff\" size=\"80\"></p>";
        echo "<p>CALENDAR</p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
 	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Last saw hosts":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Host name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host address: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"address\"></p>";
	echo "<p>Host's unique key:<input class=\"searchfield\" type=\"text\" name=\"key\"></p>";
	echo "<p>Hours ago:<p><input class=\"searchfield\" type=\"text\" name=\"ago\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";


	break;
	
      case "Patches available":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>package version: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"version\"></p>";
	echo "<p>Package architecture:<p><input class=\"searchfield\" type=\"text\" name=\"arch\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Patch status":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>package version: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"version\"></p>";
	echo "<p>Package architecture: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"arch\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";
	
	break;
	
      case "Performance":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type:$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Job name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";
	
	break;
	
      case "Promises repaired log":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Promises not kept log":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Promise handles: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Setuid/gid root programs":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Filename: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\"></p>";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Software installed":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Package name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"name\"></p>";
	echo "<p>package version: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"version\" size=\"80\"></p>";
	echo "<p>Package architecture: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"arch\" size=\"80\"></p>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
	
      case "Variables":
	echo "<div class=\"panel\"><div class=\"panelhead\">$report_type query</div><div class=\"panelcontent\">";
	echo "<form method=\"post\" action=\"search.php\">";
	echo "<p>Scope or bundle: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"scope\"></p>";
	echo "<p>Lvalue or name: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"lval\"></p>";
	echo "<p>Rvalue or content: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"rval\"></p>";
	echo "<p>Type:<p>";
	echo "<select name=\"type\"><option value=\"s\">String</option><option value=\"sl\">String list</option><option value=\"i\">Integer</option><option value=\"il\">Integer list</option><option value=\"r\">Real</option><option value=\"rl\">Real list</option></select>";
	echo "<p>Host group: (.*+[])<input class=\"searchfield\" type=\"text\" name=\"class_regex\" size=\"80\"></p>";
	echo "<input type=\"hidden\" name=\"manyhosts\" value=\"true\">";
	echo "<input type=\"hidden\" name=\"report\" value=\"$report_type\">";
	echo "<p>Return hostnames only: <input type=\"checkbox\" name=\"hosts_only\" value=\"true\"></p>";
	echo "<p><input type=\"submit\" value=\"Generate report\"></p>";
	echo "</form></div></div>";

	break;
      }
   echo "</div><div class=\"grid_7\"><div id=\"tips\"><img src=\"images/Lamp.png\" />
    <p> Use regular expressions that match the <b>whole string</b> you want to find
    <a href=\"knowledge.php?topic=regular expression\">Learn about regular expressions</a></p>
   <div class=\"clearleft\"></div></div></div>";
   echo "<div class=\"clear\"></div>";
  }
?>
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
//cfpr_footer();
include 'footer.php';
?>



