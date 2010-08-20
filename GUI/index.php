<?php
cfpr_header("overview","normal");
?>

<div id="primary">
<?php

$ret1 = cfpr_getlicense_owner();
$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();

$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts();

echo "<div id=\"selector\">";
echo "<table><tr><td>\n";
echo "<ul>\n";

echo "<li><h2>$all hosts are under management</h2>";
echo "<li><a href=\"hosts.php?type=red\"><img src=\"/img/red_sign_big.png\"> $r hosts known</a>";
echo "<li><a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow_sign_big.png\"> $y hosts known</a>";
echo "<li><a href=\"hosts.php?type=green\"><img src=\"/img/green_sign_big.png\"> $g hosts known</a>";
echo "</ul>\n";
echo "<ul><li>This edition licensed to `<b>$ret1</b>'. <li>Granted $ret2 licenses. <li>You promise to have paid for $ret3 paid</ul>";

echo "</td><td>\n";
cfpr_compliance_summary_graph();
echo "<ul><li><img src=\"/hub/common/compliance.png\">";                                                               
echo "<br>";
echo "<li><img src=\"/green.png\"> promises kept <li><img src=\"/yellow.png\">repaired <li><img src=\"/red.png\"> not kept";
echo "</ul></td></tr></table>";
echo "</div>";


cfpr_footer();
?>
