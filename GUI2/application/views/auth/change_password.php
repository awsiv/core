<div class="panelhead">Change password</div>

<div id="infoMessage"><?php echo $message;?></div>

<?php echo form_open("auth/change_password/".$this->uri->segment(3), array('id'=>'change_password'));?>

      <p>Old Password:<br />
      <?php echo form_input($old_password);?>
      </p>
      
      <p>New Password:<br />
      <?php echo form_input($new_password);?>
      </p>
      
      <p>Confirm New Password:<br />
      <?php echo form_input($new_password_confirm);?>
      </p>

      <p><?php echo form_submit('submit', 'Change');?></p>

<?php echo form_close();?>