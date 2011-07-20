<div class="outerdiv grid_12">
    <div class="innerdiv">
	 <p class="title">
                <strong><?php echo $report_type?> Report</strong>  
         </p>
         <div><?Php
               $link="";
               if($classregex) { echo "Host list according to  <strong>Class :</strong>  $classregex"; }
              if(isset($name)) {$link.="  <strong>Name:</strong> ".$name; }
               echo $link;
              ?>
         </div>
         <!--<h2>Matched Hosts</h2>-->
		   <div class="tables tablesfixed">
			<?php 
                        $data=json_decode($report_result,true);
                        echo  host_only_table($data);
                        ?>
                   </div>
    </div>
</div>