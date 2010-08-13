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

echo "<b>$result</b>";
 
echo "</div>";

cfpr_footer();
?>