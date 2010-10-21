<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

$view =  $_GET['view'];
cfpr_header("vital signs","normal");
$hostkey = $_POST['hostkey'];
$report_type = $_POST['report'];
if ($hostkey == "")
   {
   $hostkey = $_GET['hostkey'];
   $report_type = $_GET['report'];
   }
$obs =  $_GET['obs'];
$obs_name = cfpr_get_observable_name($obs);
$nm =  $_GET['nm'];
?>
<link href="css/jquery.fancybox-1.3.1.css" rel="stylesheet" media="screen" />
<?php
cfpr_menu("Status : vital signs");
?>
        
        <div id="tabpane">
            <div class="pagepanel">
               <div class="panelhead">Vital signals </div>
               <div class="panelcontent">
                 <div class="vitaltable">
                <?php
                $ret = cfpr_performance_analysis($hostkey);
                echo $ret;
                ?>
                 </div>
                </div>
            </div>
        </div>
     <script type="text/javascript" src="scripts/jquery.fancybox-1.3.1.js"></script>
     <script type="text/javascript">
	 $(document).ready(function() {
	 $("#tabpane a:has(img)").click(function(e)
		{
	    e.preventDefault();
		var oldlink=$(this).attr('href');
		var newlink=oldlink.split('?');
		 $.fancybox({
		 'padding' : 0,
		 'href' : 'vitaldetails.php?'+newlink[1], 'title' : 'Details',
		 'type':'ajax',
		 'transitionIn' : 'elastic',
		 'transitionOut' : 'elastic'
		});
	 });
   });
	 </script>
<?php
cfpr_footer();
?>