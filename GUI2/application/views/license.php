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
                     $data=json_decode($txt,true);
                     var_dump($txt);
                     foreach ($data as $key->$value)
                      {
                         $this->table->add_row($key, $value);
                      }
                     echo $this->table->generate();
                      ?>
               </div>
        </div>
        </div>
 