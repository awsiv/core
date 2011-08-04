<div class="outerdiv">
    <div class="innerdiv">
          <p class="title"><?php echo $report; ?></p>
          <div class="tables">
        <?php
         if((int)$tabledata['meta']['count'] >0){
          $pg = paging($current, $number_of_rows, $tabledata['meta']['count'], 10);
          $report_table=$this->cf_table->generate_cdp_report_table($report,$tabledata);
           echo $report_table;
           include 'paging_footer.php';
         }else{
               echo"<table><tr><td>" . $this->lang->line("no_data") . "</td></tr></table>";
         }
          ?>
          </div>
    </div>
</div>
