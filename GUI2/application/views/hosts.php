<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
         
     <div id="body" class="outerdiv grid_12">
                <div id="" class="innerdiv tables hostlist">
                   

      <?php
      if($count>0)
      {
          ?>
                     <p class="hostlisttitle">
                    <span class="<?php echo $type.'host'?> coloriconinline"></span><span class="title"><?php echo $this->lang->line($type )?> hosts</span>
                    </p>
       <?php
           echo $tabledata;
                $pg = paging($current, $number_of_rows, $count, 10);?>
                <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <?php 
                        $first = site_url("welcome/hosts/$type/$params/rows/$number_of_rows/page/" . $pg['first']);
                        $prev = site_url("welcome/hosts/$type/$params/rows/$number_of_rows/page/". $pg['prev']);
                        $next = site_url("welcome/hosts/$type/$params/rows/$number_of_rows/page/" . $pg['next']);
                        $last= site_url("welcome/hosts/$type/$params/rows/$number_of_rows/page/" . $pg['last']);
                        ?>
                        <a href="<?php echo $first?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?php echo $prev?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?php
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $selected = 'current'; else
                                $selected="";
                        ?>

                            <a href="<?php echo  site_url("welcome/hosts/$type/$params/rows/$number_of_rows/page/$i") ?>" title="Go to Page <?php echo  $i ?>" class="page <?php echo  $selected ?>"><span><?php echo  $i ?></span></a>

                        <?php  } ?>

                        <a href="<?php echo $next ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo $last ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>
                <div>
                    <?php
                    echo form_open('welcome/hosts/'.$type.'/'.$params);
                    echo form_input('rows', $number_of_rows);
                    echo "Rows/Page";
                    echo form_close();
                    ?>
                </div>
            </div>
       <?php }

       else
       {
         echo "<span class=\"nodata\">".$this->lang->line('no_data')."</span>";
       }
       ?>
</div>
</div>
<div id="hostdelconfirmation" title="Proceed Host Deletion" style="display:none">
    <span>
        <?php
        echo $this->lang->line('host_delete_confirm');
        ?>
    </span>
</div>
<script type="text/javascript">
    
    $(document).ready(function() {
     
     /* $.tablesorter.addParser({
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
       
   */

    $("a.deletehostbtn").bind("click", function(event){
         var a = this; 
          $('#hostdelconfirmation').dialog({
               modal: true,
               width: 400,
               resizable:false,
                buttons: {
                  "Yes": function() {
                            $(this).dialog('close');
                            window.location = a.href; 
                  },
                  'NO': function() {
	               $(this).dialog('close');
	         }
                 }
            }); 
        return false;
        });  
    });

   </script>