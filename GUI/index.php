<?php
$title = "mission overview";
?>

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
          <a href="/index.php"><h2>CFENGINE NOVA <span id="subheading"><b><?php echo "$title";?></b></span></h2></a>
          </div>

<div id="banner">
<ul>
<li><a href="index.php"><b>Mission overview</b><br></a>          
<li><a href="knowledge.php"><b>Knowledge</b><br>Science</a>
<li><a href="helm.php"><b>Mission control</b><br>Helm</a>
<li><a href="status.php"><b>Mission status</b><br>Engineering</a>
<li><a href="security.php"><b>Mission integrity</b><br>Security</a>
<li><a href="communications.php"><b>Mission log</b><br>Communications</a>
</ul>

</div><br>
          
<div id="primary">
                                                                             
<?php

$ret1 = cfpr_getlicense_owner();
$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();

$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();

echo "<div id=\"info\"><ul>\n";

echo "<h2>$ret1  $ret2 licenses $ret3 paid</h2>";

echo "<li>CURRENT STATE: $all hosts known";
echo "<li><img src=\"/img/red_sign_big.png\"> $r hosts known";
echo "<li><img src=\"/img/yellow_sign_big.png\"> $y hosts known";
echo "<li><img src=\"/img/green_sign_big.png\"> $g hosts known";
echo "</ul></div>\n";


cfpr_summary_meter(NULL);

echo "<div id=\"meter\"><img src=\"/hub/common/meter.png\"></div>";
?>
          


?>

<div id="footer">Copyright &copy; Cfengine AS</div></div></body></html>