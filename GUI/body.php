<?php

$body = $_GET['body'];
$type = $_GET['type'];

cfpr_header("body $body","normal");

echo "<div id=\"showbody\">";

echo "<table>\n";
echo "<tr><td width=\"30%\" valign=\"top\">";

echo "<h2>Other bodies of type $type</h2>";

$allbodies = cfpr_list_bodies(".*",$type);

echo "$allbodies";

echo "</td><td valign=\"top\">";


echo "<h2>Body definition:</h2>";

$def = cfpr_get_promise_body($body,$type);

echo "<p>$def";

echo "</td></tr></table></div>\n";

cfpr_footer();
?>