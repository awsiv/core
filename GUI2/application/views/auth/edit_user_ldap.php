
<div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
    <?php echo form_open("auth/edit_user_ldap/".$this->uri->segment(3),array('id'=>'edit_user'));?>
    
      <p>
       <?php
       echo form_label('User Name',$user_name['name']);
       echo form_input($user_name);?>
      </p> 
      
      <p>
      <?php //echo form_dropdown($role['name'],$role['options'],$role['default']);
       echo form_label('Role');
       foreach($roles as $role)
        {
          echo form_checkbox($role);
          echo form_label($role['id'], $role['id'],array('class'=>'role'));
        }
      ?>
      </p>
      
      <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Update'));?></p>
    <?php echo form_close();?>
  </div>


