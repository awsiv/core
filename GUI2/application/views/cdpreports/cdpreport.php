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
               echo"<div class='info' style='margin:10px;'>" . $this->lang->line("no_data") . "</div>";
         }
          ?>
          </div>
    </div>
</div>
<script type="text/javascript">
    $(document).ready(function() {
          $.tablesorter.addParser({
      id: 'hostname',
      is: function(s) {
        return false;
      },
      format: function(s) {
        var str = s.replace(/(\d{1,})/g, function(a){
            return pad(a);
        });

        return str;
      },
      type: 'text'
    });

    function pad(num ){
      var s = '00000' + num;
      return s.substr(s.length-5);
    }
        $('.tables table').tablesorter({
            widgets: ['zebra'],
            headers: {0: {sorter:'hostname'} } ,
            sortList: [[0,0]]
       });
    });
</script>
