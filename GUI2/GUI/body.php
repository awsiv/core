<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

$body = $_GET['body'];
$type = $_GET['type'];
cfpr_header("body $body","normal");
cfpr_menu("Show : Body definition");
?>
     <div id="tabpane">
         <div class="grid_5">
<?php
             echo "<div class=\"panel\"><div class=\"panelhead\">Other bodies of type $type</div>";
             echo "<div class=\"panelcontent\">";
             $allbodies = cfpr_list_bodies(".*",$type);
             echo "$allbodies";
             echo "</div></div>";
?>
         </div>
         <div class="grid_7">
             <?php
             echo "<div class=\"panel\"><div class=\"panelhead\">Body definition:</div>";
             echo "<div class=\"panelcontent\"><div class=\"tables\">";
             $def = cfpr_get_promise_body($body,$type);
             echo "$def";
             echo "</div></div></div>";
             ?>
         </div>
     </div>

<?php
cfpr_footer();
?>