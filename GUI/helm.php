<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("control","normal");
cfpr_menu("Planning : menu");
?>
       
<div id="tabpane">
      <div class="pagepanel">
          		<div class="panelhead"> Mission control and planning</div>
                <div class="panelcontent">
     
    <ul class="grid_6">
        <li><a href="services.php"><img src="images/goal.png" class="align" /><span class="imglabel">Enterprise goals and Service Catalogue</span></a></li>
        <li><a href="Cfeditor.php"><img src="images/tool.png" class="align" /><span class="imglabel">Develop and work on policy</span></a></li>
        <li><a href="knowledge.php?topic=security view"><img src="images/security.png" class="align" /><span class="imglabel">Security</span></a></li>
    </ul>
    <ul class="grid_6">
        <li><a href="search.php?report=Business%20value%20report&manyhosts=1"><img src="images/info_services.png" class="align" /><span class="imglabel">Latest enterprise cost and value</span></a></li>
        <li><a href="knowledge.php?topic=best.*practice"><img src="images/library.png" class="align" /><span class="imglabel">Standards and practices</span></a></li>
        <li><a href="knowledge.php?topic=policy.*"><img src="images/library.png" class="align" /><span class="imglabel">Policy guidance</span></a></li>
    </ul>
    <div class="clearleft"></div>
    </div> 
    </div>
   </div>

<?php
cfpr_footer();
?> 