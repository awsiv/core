<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

?>
        
        <div class="outerdiv grid_12">
        <div class="innerdiv">
         <p class="title">License summary</p>
		  <ul>
		   <li>Granted <?php echo $ret2?> licenses.</li>
                   <li>Purchased on: <?php echo $started?></li>
		   <li>Expiring on: <?php echo $expiry?></li>
		  </ul>
               <div class="tables">
                   
		     <?php
  
                     //echo $txt; remove the newline character
                     $txt = str_replace(array("\r\n", "\n", "\r"), '', $txt);
                     $data=json_decode(utf8_encode(trim($txt)));
                     //var_dump($data);
                    // echo json_last_error();
                     foreach ((array)$data as $key=>$value)
                      {
                         $this->table->add_row($key, $value);
                      }
                     echo $this->table->generate();
                      ?>
               </div>
          <div id="licensebar" class="grid_4 push_4">
              <div></div>
          </div>
         <div class="clear"></div>
        </div>
        </div>
<script type="text/javascript">
    $('#licensebar').find('div').licensemeter(
             {
                 value:<?php echo $pbarvalue?>,
                 daysleft:<?php echo $daysleft?>
             });
 </script>
 