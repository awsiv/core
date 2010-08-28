<?php

$bundle = $_GET['bundle'];

cfpr_header("bundle $bundle","normal");

echo "<div id=\"showbundle\">";

echo "<table>\n";
echo "<tr><td width=\"50%\" valign=\"top\">";

$allbundles = cfpr_list_all_bundles(NULL);

echo "$allbundles";

echo "</td><td valign=\"top\">";


echo "<h2>Bundle definition of $bundle:</h2>";

echo "<p>Name: <span id=\"bundle\">$bundle</span><br>";

$args = cfpr_get_args_for_bundle($bundle,NULL);
echo "<p>Arguments: <span id=\"args\">$args</span>";

$classes = cfpr_get_classes_for_bundle($bundle,NULL);
echo "<p>Host classes using this bundle: $classes";

$list = cfpr_list_handles_for_bundle($bundle,"agent",false);
echo "<p>Promises in this bundle $list";

$others = cfpr_list_bundles_using($bundle);

echo "<p>Bundles using this bundle: $others";


echo "</td></tr></table></div>\n";

cfpr_footer();
?>