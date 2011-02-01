<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("compliance","normal");
cfpr_menu("Status : Promises");
$hostkey = NULL;
$name = ".*";
$regex = 1;
$hash = NULL;
$host = NULL;
$addr = NULL;
$handle = NULL;
$tago = 0;
$status = NULL;
?>
        <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Promises</div>
                <div class="panelcontent">
                <div class="tables">
				<?php
                $ret = cfpr_report_compliance_promises($hostkey,$handle,"x",false,NULL);
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
