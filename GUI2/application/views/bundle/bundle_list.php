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
	var oldlink=$("#tabpane table a").attr('href');
	var newlink=oldlink.split('?');
	var opt=newlink[1].replace('/&/','');
    opt=opt.split('=');
	opt.join('\/');
	$("#tabpane table a").attr('href','<?php echo site_url()?>/bundle/details/'+opt);
//the links generated in the table must be like site_url/bundle/details/key/value/key/value/key/value
});
</script>