<div id="body">
             
              <div id="work" class="outerdiv">
                  <div class="grid_8 push_2 header">
                      <h1>
                          <?php echo  $this->lang->line('front_greeting');?>
                      </h1>
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

                  <div id="licenseinfo"class="push_4"><?php
                      echo $this->lang->line('license_to');
                      echo  anchor('welcome/license',cfpr_getlicense_owner(),array('class'=>'licenseinfo', 'title'=>"view license information"));?></div>
                  <div class="clear"></div>
                  <div id="licensebar" class="grid_4 push_4">
                  </div>
                   <div class="clear"></div>
              </div>
  </div>
<script type="text/javascript">
    $('#licensebar').licensemeter(
             {
                 value:<?php echo $pbarvalue?>,
                 daysleft:<?php echo $daysleft?>
             });

 /* var socket = new io.Socket(null, {port: 8080});
   socket.connect();
	  socket.on('message', function(msg){
          var data=JSON.parse(msg);
          if(data.event_loggedin)
              {
                 console.log(data.ttlusr);
                 $('#ttlonlinenum').html(data.ttlusr);
              } 
 });*/
 </script>

