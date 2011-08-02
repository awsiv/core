<div class="outerdiv">
    <div class="innerdiv">
          <p class="title"><?php echo $report; ?></p>
          <div class="tables">
        <?php
         if((int)$tabledata['meta']['count'] >0){
          $report_table=$this->cf_table->generate_cdp_report_table($report,$tabledata);
           echo $report_table;
         }else{
               echo"<table><tr><td>" . $this->lang->line("no_data") . "</td></tr></table>";
         }
          ?>
          </div>
    </div>
</div>
