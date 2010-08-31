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
echo "<li><a href=\"hosts.php?type=red\"><img src=\"/img/red_sign_big.png\"> $r hosts known</a>";
echo "<li><a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow_sign_big.png\"> $y hosts known</a>";
echo "<li><a href=\"hosts.php?type=green\"><img src=\"/img/green_sign_big.png\"> $g hosts known</a>";
echo "</ul>\n";
echo "<ul><li>This edition licensed to `<b>$ret1</b>'.";
echo "<li><a href=\"license.php\"><img src=\"/img/info.png\"></a></ul>";

echo "</td><td valign=\"top\">\n";
cfpr_compliance_summary_graph();

echo "<h2>compliance promise summary</h2><br><br>";
echo "<table id=\"noborder\"><tr><td valign=\"top\">";
echo "<img src=\"/hub/common/compliance.png\">";                                                               
echo "</td><td width=\"50%\" valign=\"top\">";
echo "<ul><li><img src=\"/green.png\"> kept <li><img src=\"/yellow.png\"> repaired <li><img src=\"/red.png\"> not kept<br>";
echo "<li><img src=\"/img/orange.png\"> no data</ul></td></tr></table></td></tr></table>";
echo "</div>";


cfpr_footer();
?>
