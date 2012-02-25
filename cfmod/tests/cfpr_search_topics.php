<?php

$userName = getenv("CFENGINE_TEST_USERNAME");
$search = "copy_from";
$regex = true;

$res = cfpr_search_topics($userName, $search, $regex);
fwrite(STDERR, $res);
?>
