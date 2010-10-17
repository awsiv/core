<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("Cfengine example","");
$example = $_GET['example'];
$docroot = cfpr_docroot();
cfpr_menu("Knowledge : example");
?>
      <div id=tabpane>
		  <?php
          include("$docroot/docs/$example");
          ?>
      </div>
<?php
cfpr_footer();
?>