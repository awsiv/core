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

if (count($tabledata)>0)
   {
    //$col_list = $this->table->make_columns($tabledata, 6);
    //echo $this->table->generate($col_list);
    echo "<table><tr>";
    for($i=0;$i<count($tabledata);$i++)
    {
        if($i%6==0 && $i!=0)
        {
            echo '</tr><tr>';
        }
        echo '<td>'.$tabledata[$i].'</td>';
    }
    echo "</tr></table>";
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