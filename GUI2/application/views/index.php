<div id="bodyindex">
              <div class="left"></div>
              <div id="work" class="middle">
                  <div class="grid_8 push_2 header">
                      <h1>
                          <?php echo  $this->lang->line('front_greeting');?>
                      </h1>
                      <p><?php
                      echo $this->lang->line('license_to');
                      echo  anchor('welcome/license',cfpr_getlicense_owner(),array('class'=>'licenseinfo'));?></p>
                  </div>
                  <div class="clear"></div>
                  <div class ="grid_8 push_2 dash">
                      <ul>
                          <li><?php echo anchor('welcome/status','status',array('class'=>'status'));?></li>
                          <li><?php echo anchor('welcome/engg','engineering',array('class'=>'engineering'));?></li>
                          <li><?php echo anchor('welcome/planning','planning',array('class'=>'planning'));?></li>
                          <li><?php echo anchor('knowledge/index','library',array('class'=>'library'));?></li>
                          <p class="clearleft"></p>
                      </ul>
                  </div>                  
                 <div class="clear"></div>
                  <div id="licensebar" class="grid_4 push_4">
                      
                  </div>
              </div>
              <div class="right"></div>
     <div class="clearboth"></div>
  </div>
<script type="text/javascript">
    $('#licensebar').licensemeter(
             {
                 value:<?php echo $pbarvalue?>,
                 daysleft:<?php echo $daysleft?>
             });

 </script>

