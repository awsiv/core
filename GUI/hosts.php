<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

$type = $_GET['type']; 
cfpr_header("$type hosts","normal");
cfpr_menu("Status : hosts");
?>
        <div id="tabpane">
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
   echo "<img src=\"/img/info.png\" class=\"floatLeft align\"><span class=\"text\">No hosts in this category</span>";
   echo "<span id=\"colourbuttons\" class=\"floatRight\"><a href=\"hosts.php?type=green\"><img src=\"/img/green.png\" class=\"align\"></a> <a href=\"hosts.php?type=yellow\"><img src=\"/img/yellow.png\" class=\"align\"></a> <a href=\"hosts.php?type=red\"><img src=\"/img/red.png\" class=\"align\"></a></span><div class=\"clear\"></div></div>";
   }
?>

</div>
</div>
</div>
</div>

<?php
cfpr_footer();
?>