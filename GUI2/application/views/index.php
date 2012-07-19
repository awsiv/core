<div id="body">
             
              <div id="work" class="outerdiv">
                  <div class="grid_8 push_2 header">
                      <h1>
                          <?php //echo  $this->lang->line('front_greeting');?>
                      </h1>
                  </div>
                  <div class="clear"></div>
                  <?php if ($user_auth_mode != $mp_auth_mode) { ?>
                  <?php 
                  
                        switch ($mp_auth_mode) {
                            case 'ldap': $mp_auth_mode = 'LDAP';
                                break;
                            case 'active_directory': $mp_auth_mode = 'Active Directory';
                                break;
                            default: $mp_auth_mode = 'Database';
                        }

                        switch ($user_auth_mode) {
                            case 'ldap': $user_auth_mode = 'LDAP';
                                break;
                            case 'active_directory': $user_auth_mode = 'Active Directory';
                                break;
                            default: $user_auth_mode = 'Database';
                        }
                  ?>
                  <div class="info" style="width: 50%; margin: 20px auto; text-align: center">
                      <p>Warning. We can't authenticate you with <?php echo $mp_auth_mode ?> authentication.</p>
                      <p>Fallback authentication method (<?php echo $user_auth_mode ?>) is used.</p>
                  </div>
                  
                  <?php } ?>
                  <div class="grid_8 push_2 dash">
                      <ul>
                          <li class="showqtip engineering" title="<?php echo tooltip('tooltip_engineeringroom')?>"><?php echo anchor('engineering',' ',array('class'=>'ico'));?><?php echo anchor('engineering','Engineering',array('class'=>'text'));?></li>
                          <li class="showqtip status" title="<?php echo tooltip('tooltip_statusroom') ?>"><?php echo anchor('welcome/status',' ',array('class'=>'ico'));?><?php echo anchor('welcome/status','Business',array('class'=>'text'));?></li>
                          <li class="showqtip planning" title="<?php echo tooltip('tooltip_planningroom')?>"><?php echo anchor('welcome/planning',' ',array('class'=>'ico'));?><?php echo anchor('welcome/planning','Planning',array('class'=>'text'));?> </li>
                          <li class="showqtip library" title="<?php echo tooltip('tooltip_libraryroom')?>"><?php echo anchor('knowledge/index',' ',array('class'=>'ico'));?><?php echo anchor('knowledge/index','Library',array('class'=>'text'));?></li>
                          <p class="clearleft"></p>
                      </ul>
                  </div>                  
                 <div class="clear"></div>

                  <div id="licenseinfo"class="push_4"><?php
                      echo $this->lang->line('license_to');
                      echo  anchor('welcome/license',$licenseOwner,array('class'=>'licenseinfo showqtip', 'title'=>tooltip('tooltip_license_report')));?></div>
                  <div class="clear"></div>
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

