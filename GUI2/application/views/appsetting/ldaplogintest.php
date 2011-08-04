<div class="form">
<?php // Change the css classes to suit your needs
$attributes = array('class' => '', 'id' => '');
echo form_open('settings/logintest', $attributes,$configsettings); 
include 'loginfields.php';
echo form_close(); ?>
</div>

