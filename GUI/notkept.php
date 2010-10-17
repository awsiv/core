<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("promises not kept","normal");
cfpr_menu("Status : promises not kept");
?>

<div id="selector">

<?php
$ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex,NULL);
?>


</div>
     
<?php
cfpr_footer();
?>