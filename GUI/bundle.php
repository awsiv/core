<?php

$bundle = $_GET['bundle'];
$type = $_GET['type'];

if ($type == "")
   {
   $type = "agent";
   }

cfpr_header("bundle $bundle","normal");

echo "<div id=\"showbundle\">";

echo "<table>\n";
echo "<tr><td width=\"40%\" valign=\"top\">";

echo "<h2>All bundles</h2>";
$allbundles = cfpr_list_all_bundles(NULL);

echo "$allbundles";

echo "</td><td valign=\"top\">";


echo "<h2>Bundle definition of $bundle:</h2>";

echo "<ul>";
echo "<li><i>Name:</i> <span id=\"bundle\">$bundle</span><br>";

$args = cfpr_get_args_for_bundle($bundle,$type);
echo "<li><i>Arguments:</i> <span id=\"args\">$args</span>";

$classes = cfpr_get_classes_for_bundle($bundle,$type);
echo "<li><i>Host classes using this bundle:</i> $classes";

$list = cfpr_list_handles_for_bundle($bundle,$type,false);
echo "<li><i>Promises in this bundle:</i> $list";

$others = cfpr_list_bundles_using($bundle);

echo "<li><i>Bundles using this bundle:</i> $others";

echo "</ul>";

echo "</td></tr></table></div>\n";

cfpr_footer();
?>