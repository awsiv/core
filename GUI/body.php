<?php
$body = $_GET['body'];
$type = $_GET['type'];
cfpr_header("body $body","normal");
?>
 <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php"  class="current">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Login:Max Manus
             </span>
             <div class="clearleft"></div>
    </div>
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
  include 'footer.php';
?>