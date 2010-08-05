<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/strict.dtd"><html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
    <meta http-equiv="refresh" CONTENT="150">
    <title>system_knowledge::mission status</title>
    <link rel="stylesheet" href="/cf_enterprise.css" type="text/css" media="screen" />
    <link rel="stylesheet" href="hand_/cf_enterprise.css" type="text/css" media="handheld" />
  </head>
  <body>
<div id="top">      <div id="search">
        <form method="post" action="/index.php">
          <p><input class="searchfield" type="text" name="regex" />&nbsp;Search</p>
        </form>
      </div> 
<a href="/index.php"><h2>CFENGINE NOVA</h2></a></div>

<div id="banner">
<ul>
<li><a href="science"><b>Explore</b><br>Science</a>
<li><a href="helm"><b>Mission control</b><br>Helm</a>
<li><span id="tab"><a href="engineering"><b>Mission status</b><br>Engineering</a></span>
<li><a href="security"><b>Mission integrity</b><br>Security</a>
<li><a href="communications"><b>Mission log</b><br>Communications</a>
</ul>

</div><br>
          
<div id="primary">
                                                                             
<?php

//          $hostkey="MD5_4dda03e4f9b7547b3dc52aa463059d90";

									 //$rep12 = cfpr_performance_analysis("MD5=4dda03e4f9b7547b3dc52aa463059d90");

									 //echo "<p><h2>PERFO</h2><hr>$rep12</hr><p>";

$rep = cfpr_topN_hosts("compliance",10);

echo "TOP 10: \n<div id=\"directory\">$rep</div>\n";

$ret1 = cfpr_hostname("MD5=4dda03e4f9b7547b3dc52aa463059d90");
$ret2 = cfpr_ipaddr("MD5=4dda03e4f9b7547b3dc52aa463059d90");

echo "<h4>This is $ret1 / $ret2";

$ret1 = cfpr_getlicense_owner();
$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();

echo ", LICENSED TO $ret1,$ret2,$ret3</h4>";


cfpr_summary_meter(NULL);
?>
          
<img src="/hub/common/meter.png">
<?php

$id = 756;

include("/srv/www/htdocs/graphs/$id.map");
#echo "<img src=\"/graphs/$id.png\">";

$topic = cfpr_show_topic($id);

echo "<div id=\"occurrences\"><h2>TOPIC ($id)</h2><p>$topic</div>";

$leads = cfpr_show_topic_leads($id);

echo "<div id=\"associations\"><h2>Assocs</h2><p>$leads</p></div>";

$occ = cfpr_show_topic_hits($id);

echo "<div id=\"occurrences\"><h2>HITS</h2>$hits<p></div><hr>";
    
 /*
$ret = cf_get_all_reports("hostkey");
$ret = cf_get_all_hostkeys();
 */
/*
$ret = cfpr_refresh_png("type","name");

$ret = cfpr_know_occurrences("topic");
$ret = cfpr_know_associations("topic");
$ret = cfpr_know_other_topics("topics");
*/

$hostkey = NULL;
$name = ".*";
$version = ".*";
$arch = ".*";
$regex = 1;

#$ret = cfpr_report_software_in(NULL,"zypper",".*",".*",1);
#echo "Software $ret<br>";

#$ret = cfpr_report_patch_in($hostkey,$name,$version,$arch,$regex);

#echo "Patches: $ret<br>";
#$ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex);

#echo "Patches: $ret<br>";

$ret = cfpr_report_classes($hostkey,$name,$regex);

echo "Classes: $ret<br>";

$scope = NULL;
$lval = NULL;
$rval = NULL;
$type = NULL;

$ret = cfpr_report_vars($hostkey,$scope,$lval,$rval,$type,$regex);

echo "Variables: $ret<br>";
$thistime = -1;
$kept = -1;
$notkept = -1;
$repaired = -1;

//$ret = cfpr_report_compliance_summary($hostkey,$version,$thistime,$kept,$notkept,$repaired,">");
//echo "Complaince: $ret<br>";

$handle = NULL;
$status = "x";
$hostkey = NULL;

//$ret = cfpr_report_compliance_promises($hostkey,$handle,$status,$regex);
//echo "Promises $ret";

$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;

$ret = cfpr_report_lastseen($hostkey,$hash,$host,$addr,$tago,$regex);
echo "Lastseen $ret";

$job = NULL;

$ret = cfpr_report_performance($hostkey,$job,$regex);
echo "Perform $ret";

$file = 0;

$ret = cfpr_report_setuid($hostkey,$file,$regex);
echo "setuid $ret";

$ret = cfpr_report_bundlesseen($hostkey,$name,$regex);
echo "bundle profile $ret";

$diff = NULL;

$ret = cfpr_report_filechanges($hostkey,$name,$regex,$time,">");
echo "change $ret";
$ret = cfpr_report_filediffs($hostkey,$name,$diff,$regex,$time,">");
echo "diff $ret";

?>

<div id="footer">Copyright &copy; Cfengine AS</div></div></body></html>