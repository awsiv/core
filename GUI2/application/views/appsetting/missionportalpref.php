<div class="outerdiv">
<div class=" innerdiv ">
    <div class="panelhead">Mission Portal Settings</div>
    <div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
   <?php // Change the css classes to suit your needs

$attributes = array('class' => '', 'id' => '');
echo form_open('settings/manage/'.$op, $attributes); ?>
 <p>
        <label for="appemail">Application email <span class="required">*</span></label>
        <?php //echo form_error('appemail'); ?>
        <input id="appemail" type="text" name="appemail" maxlength="50" value="<?php echo $appemail; ?>"  />
</p>

<p>
   <label for="authentication">Authentication mode <span class="required">*</span></label>
        <?php// echo form_error('mode'); ?>
      
                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                <input id="mode1" name="mode" type="radio" class="" value="database" <?php echo (isset($database))?$database:$this->form_validation->set_radio('mode', 'ldap') ; ?> />
        		<label for="mode" class="">Database</label>

        		<input id="mode2" name="mode" type="radio" class="" value="ldap" <?php echo (isset($ldap))?$ldap:$this->form_validation->set_radio('mode', 'ldap'); ?> />
        		<label for="mode" class="">LDAP</label>
</p>


<p>
        <label for="username">username <span class="required">*</span></label>
        <?php// echo form_error('username'); ?>
        <input id="username" type="text" name="username" maxlength="50" value="<?php echo $username ;?>"  />
</p>

<p>
        <label for="password">password <span class="required">*</span></label>
        <?php //echo form_error('password'); ?>
        <input id="password" type="password" name="password" maxlength="50" value="<?php echo $password; ?>"  />
</p>

<p>
        <label for="confirm_password">confirm password <span class="required">*</span></label>
        <?php// echo form_error('confirm_password'); ?>
        <input id="confirm_password" type="password" name="confirm_password" maxlength="50" value="<?php echo $password ?>"  />
</p>

<p>
        <label for="host">LDAP host <span class="required">*</span></label>
        <?php //echo form_error('host'); ?>
        <input id="host" type="text" name="host" maxlength="50" value="<?php echo $host; ?>"  />
</p>

<p>
        <label for="base_dn">base dn <span class="required">*</span></label>
        <?php// echo form_error('base_dn'); ?>
        <input id="base_dn" type="text" name="base_dn" maxlength="50" value="<?php echo $base_dn; ?>"  />
</p>

<p>
        <label for="login_attribute">login attribute <span class="required">*</span></label>
        <?php// echo form_error('login_attribute'); ?>
        <input id="login_attribute" type="text" name="login_attribute"  value="<?php echo $login_attribute; ?>"  />
</p>

<p>
        <label for="users_directory">Users Directory <span class="required">*</span></label>
        <?php// echo form_error('login_attribute'); ?>
        <input id="users_directory" type="text" name="users_directory"  value="<?php echo $users_directory ?>"  />
</p>

<p>
        <label for="active_directory">Active directory <span class="required">*</span></label>
        <?php //echo form_error('active_directory'); ?>
      
                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                <input id="active_directory" name="active_directory" type="radio" class="" value="yes" <?php echo (isset($yes))?$yes:$this->form_validation->set_radio('active_directory', 'yes'); ?> />
        		<label for="active_directory" class="">Yes</label>

        		<input id="active_directory" name="active_directory" type="radio" class="" value="no" <?php echo (isset($no))?$no:$this->form_validation->set_radio('active_directory', 'no'); ?> />
        		<label for="active_directory" class="">No</label>
</p>


<p>
        <label for="active_directory_domain">active directory domain <span class="required">*</span></label>
        <?php// echo form_error('active_directory_domain'); ?>
        <input id="active_directory_domain" type="text" name="active_directory_domain"  value="<?php echo $active_directory_domain  ?>"  />
</p>

<p>
        <label for="member_attribute">member attribute <span class="required">*</span></label>
        <?php //echo form_error('member_attribute'); ?>
        <input id="member_attribute" type="text" name="member_attribute"  value="<?php echo $member_attribute ?>"  />
</p>


<p  id="btnholder">
        <?php echo form_submit( array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?>
</p>

<?php echo form_close(); ?>

    </div>
  </div>
</div>
