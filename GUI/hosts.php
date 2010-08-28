<?php

$type = $_GET['type'];

cfpr_header("$type hosts","normal");

echo "<div id=\"allhosts\">";

switch ($type)
  {
  case "red":
    $result = cfpr_show_red_hosts();
    break;
  case "green":
    $result = cfpr_show_green_hosts();
    break;
  case "yellow":
    $result = cfpr_show_yellow_hosts();
    break;
  }     

if ($result != "")
   {
   echo "<b>$result</b>";
   }
else
   {
   echo "<span id=\"announcement\"><table><tr><td width=\"70\"><img src=\"/img/info.png\"></td><td>No hosts in this category";
   echo "<span id=\"colourbuttons\"><a href=\"hosts.php?type=green\"><img src=\"/img/green.png\"></a> <a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow.png\"></a> <a href=\"hosts.php?type=red\"><img src=\"/img/red.png\"></a></span>";
   echo "</td></tr></table></span>";
   }

echo "</div>";

cfpr_footer();
?>