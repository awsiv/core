<?php
#
# This file is (C) Cfengine AS. All rights reserved
?>
        <div id="tabpane">
            <div class="pagepanel">
               <div class="panelhead">Vital signals </div>
               <div class="panelcontent">
                 <div class="vitaltable">
                <?php echo $ret ?>
                 </div>
                </div>
            </div>
        </div>
     <script type="text/javascript" src="<?php echo get_scriptdir()?>/jquery.fancybox-1.3.4.js"></script>
     <script type="text/javascript">
	 $(document).ready(function() {
	 $("#tabpane a:has(img)").click(function(e)
		{
	    e.preventDefault();
		var oldlink=$(this).attr('href');
		var newlink=oldlink.split('?');
		var url=newlink[1].replace(/&/ig,'/');
		url=url.split('=');
		url=url.join('/');
		 $.fancybox({
		 'padding' : 0,
		 'href' : '<?php echo site_url('visual/vitaldetail');?>/' + url,
		 'type':'ajax',
		 'transitionIn' : 'elastic',
		 'transitionOut' : 'elastic'
		});
	 });
   });
	 </script>
