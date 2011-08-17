<div class="outerdiv">
<div class=" innerdiv ">
    <div class="panelhead">Mission Portal Settings</div>
    <div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
   <?php // Change the css classes to suit your needs

$attributes = array('class' => '', 'id' => '');
echo form_open('settings/manage/'.$op, $attributes); ?>
 <p>
        <label for="appemail">Application email <span class="required"></span></label>
        <?php //echo form_error('appemail'); ?>
        <input id="appemail" type="text" name="appemail" maxlength="50" value="<?php echo $appemail; ?>"  />
</p>

<p>
   <label for="authentication">Authentication mode <span class="required"></span></label>
        <?php // echo form_error('mode'); ?>
      
                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                     <input id="mode1" name="mode" type="radio" class="" value="database" <?php echo (isset($database))?$database:$this->form_validation->set_radio('mode', 'database') ; ?> checked="checked"/>
        		<label for="mode" class="">Database</label>

        		<input id="mode2" name="mode" type="radio" class="" value="ldap" <?php echo (isset($ldap))?$ldap:$this->form_validation->set_radio('mode', 'ldap'); ?> />
        		<label for="mode" class="">LDAP</label>

                       <input id="mode3" name="mode" type="radio" class="" value="active_directory" <?php echo (isset($active_directory))?$active_directory:$this->form_validation->set_radio('mode', 'active_directory') ; ?> />
        		<label for="mode" class="">Active Directory</label>
</p>

<fieldset id="ldapsettings">
    <legend></legend>
<p>
        <label for="host">LDAP host <span class="required"></span></label>
        <?php //echo form_error('host'); ?>
        <input id="host" type="text" name="host" maxlength="50" value="<?php echo $host; ?>"  />
</p>

<p>
        <label for="base_dn">base dn <span class="required"></span></label>
        <?php // echo form_error('base_dn'); ?>
        <input id="base_dn" type="text" name="base_dn" maxlength="50" value="<?php echo $base_dn; ?>"  />
</p>

<p>
        <label for="login_attribute">login attribute <span class="required"></span></label>
        <?php // echo form_error('login_attribute'); ?>
        <input id="login_attribute" type="text" name="login_attribute"  value="<?php echo $login_attribute; ?>"  />
</p>

<p>
        <label for="users_directory">Users Directory <span class="required"></span></label>
        <?php // echo form_error('login_attribute'); ?>
        <input id="users_directory" type="text" name="users_directory"  value="<?php echo $users_directory ?>"  />
</p>

<p id="member_attribute_related">
        <label for="member_attribute">member attribute <span class="required"></span></label>
        <?php //echo form_error('member_attribute'); ?>
        <input id="member_attribute" type="text" name="member_attribute"  value="<?php echo $member_attribute ?>"  />
</p>


<p id="adrelated">
        <label for="active_directory_domain">active directory domain <span class="required"></span></label>
        <?php // echo form_error('active_directory_domain'); ?>
        <input id="active_directory_domain" type="text" name="active_directory_domain"  value="<?php echo $active_directory_domain  ?>"  />
</p>

<p><label></label> <a class="btn" id="testsettings" href="<?php echo site_url('settings/ldaptest')?>">Test it </a></p>
</fieldset>
<p>
    <label for="fall back for">Fall back for group ( if Authentication Server Down)<span class="required"></span></label>
    <?php echo form_dropdown('fall_back_for', $groups, $fall_back_for?$fall_back_for:'select');?>
</p>

 <?php if(isset( $groupsacc)){?>
<p>
    <label for="fall back for">Admin Group<span class="required"></span></label>
   <?php  echo form_dropdown('admin_group', $groupsacc, $admin_group?$admin_group:'select');?>
</p>
<?php }?>


<p  id="btnholder">
        <?php echo form_submit( array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?>
</p>

<?php echo form_close(); ?>

    </div>
  </div>
</div>
<script type="text/javascript">
 $(document).ready(function(){
     $('#ldapsettings').hide();
      settings_toggle();
        function settings_toggle(){
            if ($("input[@name='mode']:checked").val() == 'database')
            {
                $('#ldapsettings').hide();
            }
            else if ($("input[@name='mode']:checked").val() == 'ldap')
            {
                $('#ldapsettings').show();
                $('#adrelated').hide();
                $('#member_attribute_related').show();
                
            }
            else if($("input[@name='mode']:checked").val() == 'active_directory'){
                $('#ldapsettings').show()
                $('#adrelated').show();
                $('#member_attribute_related').hide();
            }
     }
     
     $("input[name='mode']").change(function(){
      settings_toggle();
      $("select[name='admin_group']").find('option') .remove().end()
     });

      $('#testsettings').bind('click',function(event){
          event.preventDefault();
          $(this).ajaxyDialog({title:'LDAP Test',clickData:{
             'mode':$("input[@name='mode']:checked").val(),
             'host':$("#host").val(),
             'basedn':$("#base_dn").val(),
             'login_attr':$("#login_attribute").val(),
             'user_dir':$("#users_directory").val(),
             'member_attr':$("#member_attribute").val(),
             'addomain':$("#active_directory_domain").val()
         }}).ajaxyDialog('open');

      });

});
</script>
