<div class="form">
<?php // Change the css classes to suit your needs
$attributes = array('class' => '', 'id' => '');
echo form_open('settings/logintest', $attributes,$configsettings); ?>
<p>
        <label for="username">username</label>
        <?php // echo form_error('username'); ?>
        <input id="username" type="text" name="username" maxlength="50" value=""  />
</p>

<p>
        <label for="password">password</label>
        <?php //echo form_error('password'); ?>
        <input id="password" type="password" name="password" maxlength="50" value=""  />
</p>
<p  id="btnholder">
        <label></label><?php echo form_submit( array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?>
</p>
<?php echo form_close(); ?>
</div>

