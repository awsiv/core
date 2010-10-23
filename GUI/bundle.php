<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

$bundle = $_GET['bundle'];
$type = $_GET['type'];

if ($type == "")
   {
   $type = "agent";
   }

cfpr_header("bundle $bundle","normal");
cfpr_menu("Show : Bundle definition");

?>
      <div id="tabpane">
          <div class="grid_5">
           <div class="panel">
             <div class="panelhead">ALL BUNDLES</div>
              <div class="panelcontent">
          <?php
          $allbundles = cfpr_list_all_bundles(NULL);
          echo "$allbundles";
          ?>
              </div>
            </div>
          </div>
      <div class="grid_7">
        <div class="panel">
<?php
	        echo "<div class=\"panelhead\">Bundle definition of $bundle:</div>
                     <div class=\"panelcontent\">";
			echo "<ul>";
			echo "<li><i>Name:</i> <span id=\"bundle\">$bundle</span><br>";
			
			$args = cfpr_get_args_for_bundle($bundle,$type);
			echo "<li><i>Arguments:</i> <span id=\"args\">$args</span>";
			
			$classes = cfpr_get_classes_for_bundle($bundle,$type);
			echo "<li><i>Host classes using this bundle:</i> $classes";
			
			$list = cfpr_list_handles_for_bundle($bundle,$type,false);
			echo "<li><i>Promises in this bundle:</i> $list";
			
			$others = cfpr_list_bundles_using($bundle);
			
			echo "<li><i>Bundles using this bundle:</i> $others";
			
			echo "</ul>";
?>
            </div>
          </div>
        </div>
        <div class="clear"></div>
      </div>
            
<?php
cfpr_footer();
?>