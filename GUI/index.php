<?php
cfpr_header("overview","normal");
?>

<div id="primary">
<?php

$ret1 = cfpr_getlicense_owner();

$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();

echo "<div id=\"selector\">";
echo "<table><tr><td valign=\"top\">\n";
echo "<h2>$all hosts registered</h2>";
echo "<ul>\n";
echo "<li><a href=\"hosts.php?type=red\"><img class=\"icontext\" src=\"/img/red_sign_big.png\"> $r hosts known</a>";
echo "<li><a href=\"hosts.php?type=yellow\"><img class=\"icontext\" src=\"/img/yellow_sign_big.png\"> $y hosts known</a>";
echo "<li><a href=\"hosts.php?type=green\"><img class=\"icontext\" src=\"/img/green_sign_big.png\"> $g hosts known</a>";
echo "</ul>\n";
echo "<a href=\"license.php\"><img class=\"icontext\" src=\"/img/icon_info_24x24.png\"></a> This edition licensed to `<b>$ret1</b>'.";

echo "</td><td valign=\"top\">\n";
cfpr_compliance_summary_graph();

echo "<h2>compliance promise summary</h2><br><br>";
echo "<table id=\"noborder\"><tr><td valign=\"top\">";
echo "<img src=\"/hub/common/compliance.png\">";                                                               
echo "</td><td width=\"50%\" valign=\"top\">";
echo "<ul><li><img class=\"icontext\" src=\"/img/green.png\"> kept <li><img class=\"icontext\" src=\"/img/yellow.png\"> repaired <li><img class=\"icontext\" src=\"/img/red.png\"> not kept<br>";
echo "<li><img class=\"icontext\" src=\"/img/orange.png\"> no data</ul></td></tr></table></td></tr></table>";
echo "</div>";


cfpr_footer();
?>
