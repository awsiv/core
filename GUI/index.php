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
echo "<li><img src=\"/img/red_sign_big.png\"> $r hosts known";
echo "<li><img src=\"/img/yellow_sign_big.png\"> $y hosts known";
echo "<li><img src=\"/img/green_sign_big.png\"> $g hosts known";
echo "</ul>\n";
echo "<ul><li>This edition licensed to `$ret1'. <li>Granted $ret2 licenses. <li>You promise to have paid for $ret3 paid</ul>";

echo "</td><td>\n";
echo "<img src=\"Policy_compliance_more_days.png\">";                                                               
echo "<br><br>";
echo "<img src=\"/green.png\"> promises kept <img src=\"/yellow.png\"> repaired <img src=\"/red.png\"> not kept";
echo "</td></tr></table>";
echo "</div>";


cfpr_footer();
?>
