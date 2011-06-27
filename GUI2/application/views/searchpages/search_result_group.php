<div class="outerdiv grid_12">
    <div class="innerdiv">
	 <p class="title"><?php echo $report_type?></p>
         <!--<h2>Matched Hosts</h2>-->
		   <div class="tables">
			<?php 
                        $data=json_decode($report_result,true);
                        echo  host_only_table($data);
                        ?>
                   </div>
    </div>
</div>