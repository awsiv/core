<?php
$title = "mission status";
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

<select name="mytable">
  <option>classes</option>
  <option>software_packages</option>
  <option>software_patches_avail</option>
  <option>software_patch_status</option>
  <option>variables</option>
</select>

          </form>
      </div> 
          <a href="/index.php"><h2>CFENGINE NOVA <b><span id="subheading"><?php echo "$title";?></b></span></h2></a>

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

<form>
  
</form>
                                                                             
<?php
$hostkey = NULL;
$name = ".*";
$useregex = 1;

if ($mytable == "classes")
   {
   $ret = cfpr_report_classes($hostkey,$regex,true);
   }
else if ($mytable == "software_packages")
   {
   $ret = cfpr_report_software_in($regex,NULL,".*",".*",1);
   }

echo "<br>$ret<br>";

?>
          

<div id="footer">Copyright &copy; Cfengine AS</div></div></body></html>