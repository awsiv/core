
<div class="panelhead">Edit User</div>

<div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
    <?php echo form_open("auth/edit_user/".$this->uri->segment(3),array('id'=>'edit_user'));?>
    
      <p>
       <?php
       echo form_label('User Name',$user_name['name']);
       echo form_input($user_name);?>
      </p>
      
      <p>
      <?php
       echo form_label('Email',$email['name']);
       echo form_input($email);?>
      </p>
      
      
      <p>Group<br />
      <?php //echo form_dropdown($group['name'],$group['options'],$group['default']);
       foreach($groups as $group)
        {
          echo form_checkbox($group);
          echo form_label($group['id'], $group['id']);
        }
      ?>
      </p>
      
      <p>
      	<input type=checkbox name="reset_password"> <label for="reset_password">Reset Password</label>
      </p>
     
      <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Update'));?></p>
    <?php echo form_close();?>
  </div>
