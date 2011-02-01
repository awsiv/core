<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link href="css/resets.css" rel="stylesheet" media="screen" />
<link href="css/960.css" rel="stylesheet" media="screen" />
<link href="css/style.css" rel="stylesheet" media="screen" />
<title>Vital details</title>
</head>
<body>
<div class="container_12">
  <div class="grid_12"><span id='drilldown'>Details</span></div>
  <div>
  <div class="grid_9">
<?php

$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];
if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }
$view =  $_GET['view'];
$obs =  $_GET['obs'];
$obs_name = cfpr_get_observable_name($obs);
$nm =  $_GET['nm'];
$host = cfpr_hostname($hostkey);

switch ($view)
   {
   case 'mag':
       echo "<div class=\"panel\"><div class=\"panelhead\">Past 4 hours' $obs_name on $host</div>";
       echo "<div class=\"panelcontent\">";    
       echo "<img src=\"/hub/$hostkey/$nm"."_mag.png\">";
       $ret = cfpr_analysis_mag($hostkey,$obs);
       echo"</div></div>";
       break;
   case 'week':
       echo "<div class=\"panel\"><div class=\"panelhead\">Past week's $obs_name on $host</div>";
       echo "<div class=\"panelcontent\">";        
       echo "<img src=\"/hub/$hostkey/$nm"."_week.png\">";
       $ret = cfpr_analysis_week($hostkey,$obs);
       echo"</div></div>";
       break;
   case 'hist':
       echo "<div class=\"panel\"><div class=\"panelhead\">Frequency spectrum for $obs_name on $host</div>";
       echo "<div class=\"panelcontent\">";    
       echo "<img src=\"/hub/$hostkey/$nm"."_hist.png\">";
       $ret = cfpr_analysis_hist($hostkey,$obs);
       echo"</div></div>";
       break;
   default:
   }
?>
</div>
<div class="grid_3">
          <div class="panel"><div class="panelhead">Legends</div>
               <ul class="panelcontent">
             <li><a href="hosts.php?type=red"><img src="images/green.png" class="align"/><span class="imglabel">Indicates the identification of a maximum (peak) value, to the maximum accuracy of the data ccategories.</span></a></li>
             <li><a href="hosts.php?type=yellow"><img src="images/red.png" class="align"/><span class="imglabel">Indicates the identification of a maximum (peak) value, to the maximum accuracy of the data ccategories.</span></a></li>
             <li><a href="hosts.php?type=green"><img src="images/blue.png" class="align"/><span class="imglabel">Normalized frequency of values relative to the mean.</span></a></li>
                     </ul>
             </div>
       </div>
   <div class="clear"></div>
   </div>
       <div class="tables">
       <?php echo "$ret";?>
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