<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

?>
        
        <div class="outerdiv">
        <div class="innerdiv">
         <p class="title">License summary</p>
		  <ul>
		   <li>Granted <?php echo $ret2?> licenses.</li> 
		   <li>Expiring on: <?php echo $expiry?></li>
		  </ul>
               <div class="tables">
                   
		     <?php
  
                     //echo $txt; remove the newline character
                     $txt = str_replace(array("\r\n", "\n", "\r"), '', $txt);
                     $data=json_decode(utf8_encode(trim($txt)));
                     //var_dump($data);
                    // echo json_last_error();
                     foreach ($data as $key=>$value)
                      {
                         $this->table->add_row($key, $value);
                      }
                     echo $this->table->generate();
                      ?>
               </div>
        </div>
        </div>
 