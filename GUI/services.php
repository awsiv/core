<?php

cfpr_header("service catalogue","normal");
?>

<div id="info">

<h2>Services and methods</h2>

<?php
$services = cfpr_list_all_bundles("agent");
     
echo "$services";
?>

</div>
<?php
cfpr_footer();
?>