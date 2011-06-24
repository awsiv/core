
<div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
   <?php echo form_open("auth/change_password/".$this->uri->segment(3), array('id'=>'change_password'));?>

      <p>
      <?php
      echo form_label('Old Password');
      echo form_input($old_password);?>
      </p>
      
      <p>
      <?php
     echo form_label(' New Password');
      echo form_input($new_password);?>
      </p>
      
      <p>
      <?php
      echo form_label('Confirm New Password');
      echo form_input($new_password_confirm);?>
      </p>

     <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'change'));?></p>

<?php echo form_close();?>
    </div>