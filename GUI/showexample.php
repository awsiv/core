<?php

$example = $_GET['example'];

cfpr_header("code example","ok");

$docroot = cfpr_docroot();

echo "<div id=\"example\">";

include("$docroot/docs/$example");

echo "</div>";
cfpr_footer();
?>



