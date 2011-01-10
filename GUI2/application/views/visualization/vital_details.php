<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="<?php echo get_cssdir()?>resets.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir()?>960.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir()?>style.css" rel="stylesheet" media="screen" />
<title>Vital details</title>
</head>
<body>
<div class="container_12">
  <div class="grid_12"><span id='drilldown'>Details</span></div>
  <div>
  <div class="grid_9">
    <div class="panel">
	  <div class="panelhead"><?php echo $title?></div>
       <div class="panelcontent">  
       <img src="<?php echo $path ?>">
	   </div>
	</div>
</div>
<div class="grid_3">
          <div class="panel"><div class="panelhead">Legends</div>
               <ul class="panelcontent">
             <li><a href="<?php echo site_url('welcome/hosts/red')?>"><img src="<?php echo get_imagedir()?>green.png" class="align"/><span class="imglabel">Indicates the identification of a maximum (peak) value, to the maximum accuracy of the data ccategories.</span></a></li>
             <li><a href="<?php echo site_url('welcome/hosts/yellow')?>"><img src="<?php echo get_imagedir()?>red.png" class="align"/><span class="imglabel">Indicates the identification of a maximum (peak) value, to the maximum accuracy of the data ccategories.</span></a></li>
             <li><a href="<?php echo site_url('welcome/hosts/green')?>"><img src="<?php echo get_imagedir()?>blue.png" class="align"/><span class="imglabel">Normalized frequency of values relative to the mean.</span></a></li>
                     </ul>
             </div>
       </div>
   <div class="clear"></div>
   </div>
       <div class="tables">
       <?php echo $ret?>
       </div>
 </div>
 <script type="text/javascript">
  //$('#drilldown').css({top: '0px' , left:$('body').width()/2 }).fadeIn();
  $('div.tables').css('position','absolute').hide();
  $('#drilldown').click(function(){
		$(this).css({'z-index':'10'});
		$('div.tables').css({top: '0' , left:'0', margin:'15px' , opacity:'0.9'});
		$('div.tables').slideToggle("slow");
	});
 </script>
</body>
</html>