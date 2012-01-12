

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
      
      <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Create User'));?></p>
    <?php echo form_close();?>
 </div>