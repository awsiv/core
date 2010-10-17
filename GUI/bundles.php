
<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("status","normal");
cfpr_menu("Services : this host");

$hostkey = NULL;
$name = ".*";
$regex = 1;

//$ret = cfpr_report_classes(NULL,$name,true);
//$ret = cfpr_report_software_in(NULL,NULL,NULL,NULL,0);

$hash = NULL;
$host = NULL;
$addr = NULL;
$tago = 0;
?>
         <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Bundles</div>
                <div class="panelcontent">
                <div class="tables">
				<?php
                //$ret = cfpr_report_lastseen($hostkey,$hash,$host,$addr,$tago,$regex);
                $ret = cfpr_report_bundlesseen($hostkey,$name,$regex,NULL);
                echo "$ret";
                ?>
                </div>
                </div>
           </div>
        </div>
   <script type="text/javascript">
$(document).ready(function() { 
    $('.tables table').prepend(
        $('<thead></thead>').append($('.tables tr:first').remove())
        );

    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']}); 
});
 
</script>
        
 <?php
cfpr_footer();
?>



