<?php
#
# (C) Cfengine AS, PHP driver for web map
#

# Make sure we fix PHP's handling of $ and ( which will break the data

$arg1 = $_GET['next'];
$regex = $_POST['regex'];

$arg1 = ($arg1) ? escapeshellarg($arg1) : "";
$regex = ($regex) ? escapeshellarg($regex) : "";

$ip = $_SERVER['REMOTE_HOST'];
$date = date('l jS \of F Y h:i:s A');
$email = $_SESSION['User']['email'];
$cfknow = "/usr/local/sbin/cf-know";

# Set this to your local knowledge base
$reports = "/var/cfengine/reports";
$file = "/srv/www/htdocs/enterprise_run.cf";

echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";

if ($arg1)
   {
   if ($arg1 == "'cfengine_knowledge_base::ontology'")
      {
      echo "<div id=\"ontology\">";
      include "ontology.html";
      echo "</div>";
      }
   else
      {
      system("$cfknow -t $arg1  -f $file");
      }
   }
else if (strlen($regex) > 0)
   {
   system("$cfknow -r $regex -f $file");
   }
else
   {
   system("$cfknow -t \"cfengine knowledge base\" -f $file");
   }

?>

