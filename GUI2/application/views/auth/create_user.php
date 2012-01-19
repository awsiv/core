

<div class="panelhead">Create User</div>
	<div id="infoMessage"><?php echo $message;?></div>
 <div class="form">
    <?php echo form_open("auth/create_user",array('id'=>'create_user'));?>
      <p>
     <?php echo form_label('User Name',$user_name['name']);
       echo form_input($user_name);?>
      </p>
      
      <p>
       <?php echo form_label('Email',$email['name']);
      echo form_input($email);?>
      </p>      
      
      <p>
       <?php echo form_label('Password',$password['name']);
       echo form_input($password);?>
      </p>
      
      <p>
      <?php echo form_label('Confirm Password',$password_confirm['name']);
      echo form_input($password_confirm);?>
      </p>
      
            <div class="roles_wrapper">
          <h2>Roles</h3>
            <div class="role_list">
                <h3>Assigned</h3>
                <ul id="user_roles">
                <?php 
                 $i=1;
                 $first = true;
                 $fist_item_class = ' class="first"';
                 
                 foreach($roles as $role)
                    {
                      if ($role['checked'] != '') { 
                          //$role['checked'] = false; // unc
                          echo '<li id="li_item'.$i.'" itemid="'.$i.'" '.($first === true ?  $fist_item_class : "").' >';
                              echo form_checkbox($role, '','','class="selectit"');
                              echo form_label($role['id'], $role['id'],array('class'=>'role'));
                          echo "</li>";     
                          $i++;
                          $first = false;
                      }
                    }
                ?>
                </ul>
                <div class="clearboth">&nbsp;</div>
            </div>  
            <div class="switchers">
                <a href="javascript:void(0);" id="move_left">&laquo; Left</a><br /><br />
                <a href="javascript:void(0);" id="move_right">Right &raquo;</a><br /><br />
                <div class="clearboth">&nbsp;</div>
            </div>
            <div class="role_list">
                <h3>Available</h3>                
                <ul id="all_roles">
                <?php 
                 $first = true;
                  foreach($roles as $role)
                    {
                      if ($role['checked'] == '') {
                        echo '<li id="li_item'.$i.'" itemid="'.$i.'" '.($first === true ? $fist_item_class : "").' >';
                            echo form_checkbox($role, '','','class="selectit" disabled="disabled"');
                            echo form_label($role['id'], $role['id'],array('class'=>'role'));
                        echo "</li>";     
                        $i++;
                        $first = false;
                      }
                    }
                ?>
                 </ul>
                 <div class="clearboth">&nbsp;</div>
            </div>
          <div class="clearboth">&nbsp;</div>
          </div>
      
      
      
      <?php /*
      <p>
      <?php echo form_label('role');
      //echo form_dropdown('role', $roles,set_value('role', '2'));
	       // echo form_dropdown($role['name'],$role['options'],$role['default']);
      foreach($roles as $role)
      {
          echo form_checkbox($role);
          echo form_label($role['id'], $role['id'],array('class'=>'role'));
      }
      ?>
      </p>
      */
      ?>
      <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Create User'));?></p>
    <?php echo form_close();?>
 </div>