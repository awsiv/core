<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
        
        <div id="body" class="outerdiv">
         <div class="innerdiv">
          		<p class="title">Services and methods</p>
                <div class="tables">
                <?php
               //echo $services;
               $result = json_decode($services, true);
               echo $this->cf_table->generate_nested_table($result,"Services and method");
                ?>
                </div>
           </div>
        </div>
