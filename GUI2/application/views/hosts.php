<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

//cfpr_header("$type hosts","normal");
//cfpr_menu("Status : hosts");
?>
         <div class="pagepanel">
          		<div class="panelhead"><?php echo $type?> hosts</div>
                <div class="panelcontent">
                <div class="tables">
<?php
				
switch ($type)
   {
   case "red":
       $result = cfpr_show_red_hosts();
       break;
   case "green":
       $result = cfpr_show_green_hosts();
       break;
   case "yellow":
       $result = cfpr_show_yellow_hosts();
       break;
   }     

if ($result != "")
   {
   echo "$result";
   }
else
   {
   echo "<div class=\"message\">";
   echo "<img src=\"/images/info_services.png\" class=\"floatLeft align\"><span class=\"text\">No hosts in this category</span>";
   echo "<span id=\"colourbuttons\" class=\"floatRight\"><a href=\"hosts.php?type=green\"><img src=\"/images/green.png\" class=\"align\"></a> <a href=\"hosts.php?type=yellow\"><img src=\"/images/yellow.png\" class=\"align\"></a> <a href=\"hosts.php?type=red\"><img src=\"/images/red.png\" class=\"align\"></a></span><div class=\"clear\"></div></div>";
   }
?>

</div>
</div>
</div>

        <script type="text/javascript">
		$(document).ready(function() {
		$('.tables table').find('img').addClass('align');
		$('.tables table').find('a').addClass('imglabel');
		
		$("#tabpane a").click(function(e)
		{
	       /* e.preventDefault();
		var oldlink=$(this).attr('href');
		var newlink=oldlink.split('?');
		var splitted=newlink[1].split('=');
		location.replace('<?php echo site_url(); ?>/welcome/host/'+splitted[1]+'='+splitted[2]);*/
	    });
		});
		</script>
