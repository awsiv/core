
       <!-- <p class="title">
            <strong><?php echo $report_title ?> Report</strong>  
        </p>
        <div style="padding-left: 10px;">
            <?php if ($classregex) { ?>
                <p>Host list according to  <strong>Class :</strong>  <?php echo $classregex; ?></p>
            <?php } ?>
            <?php if (isset($name)) { ?>
                <p> <strong>Name:</strong> <?php echo  $name; ?> </p>
            <?php } ?>
        </div>-->
<div class="tables hostlist">
            <?php
            if (is_array($report_result) && !empty($report_result['data'])) {
                echo  $this->cf_table->host_only_table($report_result);
                include 'paging_footer.php';
            } else {
                echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
            }
            ?>
</div>