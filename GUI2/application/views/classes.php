<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
        
        <div id="tabpane">
         <div class="pagepanel">
          		<div class="panelhead">Promises</div>
                <div class="panelcontent">
                <div class="tables">
				<?php echo "$ret"; ?>
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

        
