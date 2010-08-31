<?php
cfpr_header("license usage status","normal");


echo "<div id=\"selector\">";

$ret2 = cfpr_getlicenses_promised();
$ret3 = cfpr_getlicenses_granted();
$expiry = cfpr_getlicense_expiry();

echo "<ul><li>Granted $ret2 licenses. <li>Expiring on: $expiry</ul>";


$txt = cfpr_getlicense_summary();

echo "$txt";

echo "</div>";
cfpr_footer();
?>