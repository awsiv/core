
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

        <form method="post" action="/which.php">

   <p><input class="searchfield" type="text" name="pack" />&nbsp;Search</p>

<select name="mytable">
  <option>classes</option>
  <option>software_packages</option>
  <option>software_patches_avail</option>
  <option>software_patch_status</option>
  <option>variables</option>
</select>
        </form>

      </div> 

<a href="/index.php"><h2>CFENGINE NOVA</h2></a></div>

<div id="primary">

<h1>Which hosts (temporary)</h1>


<?php
#
# (C) Cfengine AS, PHP temporary solution
#


$regex = $_POST['pack'];
$table = $_POST['mytable'];

function lookforpattern($pattern,$in)
{
$result = "";

if ($fp = fopen("$in.html", "r"))
  {
  while (!feof($fp))
     {
     $line = fgetss($fp);
     // echo "look for $pattern in $line in $table<br>";
     
     if (preg_match("/.*$pattern.*/",$line))
	{
        $result = $result . "$line<br>";
	}
     }
  
  fclose($fp);
  }
else
   {
    //  echo "Couldn't open $in<br>";
   }

return $result;
}

# Start **********************************************************************

$docroot = file_get_contents("/var/cfengine/document_root.dat");
$dir = "$docroot/reports";

if ($regex == "")
  {
  echo"<h4>Enter a regular expression to match a name in $table</h4>";
  }
else
   {
   echo "<h4>Looking for \"$regex\" in $table</h4>";
   echo "<table>";
   
   if (is_dir($dir)) 
      {
      if ($dh = opendir($dir)) 
         {
         while (($file = readdir($dh)) !== false) 
            {
            if ($file == "." || $file == "allhosts.html" || $file == "host_portal.html" || $file == ".." | $file == "meters.png" || $file == "summary.z" || $file == "license_report.html")
               {
               continue;
               }
            
            if ($result = lookforpattern("$regex","$dir/$file/$table"))
               {
               echo "<tr><td>Host: $file</td> <td>contains \"$regex\"</td><td>$result</td></tr>\n";
               }
            }
         closedir($dh);
         }
      }
   else
      {
      echo "<h4>Unable to read the document root</h4>";
      }
   echo "</table>";   
   }

?>

<div id="footer">Copyright &copy; Cfengine AS</div></div></body></html>

