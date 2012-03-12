<div class="outerdiv grid_12">
    <div class="innerdiv">
        <p class="title">
            <strong><?php echo $report_title ?> Report</strong>  
        </p>
        <div style="padding-left: 10px;">
            <?php if ($classregex) { ?>
                <p>Host list according to  <strong>Class :</strong>  <?php echo $classregex; ?></p>
            <?php } ?>
            <?php if (isset($name)) { ?>
                <p> <strong>Name:</strong> <?php echo  $name; ?> </p>
            <?php } ?>
        </div>
        <div class="tables tablesfixed">
            <?php
            if(!is_array($report_result)){
               $data = json_decode($report_result, true); 
            }else{
                $data=$report_result;
            }
            
            if (is_array($data) && !empty($data)) {
                echo host_only_table($data);
            } else {
                echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
            }
            ?>
        </div>
    </div>
</div>