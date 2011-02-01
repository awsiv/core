

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
      <?php echo form_label('Group');
      //echo form_dropdown('group', $groups,set_value('group', '2'));
	       // echo form_dropdown($group['name'],$group['options'],$group['default']);
      foreach($groups as $group)
      {
          echo form_checkbox($group);
          echo form_label($group['id'], $group['id'],array('class'=>'group'));
      }
      ?>
      </p>
      
      <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Create User'));?></p>
    <?php echo form_close();?>
 </div>