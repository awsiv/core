<?php

$bodyType = "copy_from";
$bodyName = "remote_cp";

$res = cfpr_body_details($bodyType, $bodyName);
fwrite(STDERR, $res);
?>
