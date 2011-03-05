<div class="pagepanel">
     <div class="panelhead">Bundles</div>
          <div class="panelcontent">
                <div class="tables">
				<?php
                echo $bundle_list;
                ?>
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