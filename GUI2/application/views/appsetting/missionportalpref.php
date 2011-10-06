<div class="outerdiv">
<div class=" innerdiv ">
    <div class="panelhead">Mission Portal settings</div>
    <div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
   <?php // Change the css classes to suit your needs

$attributes = array('class' => '', 'id' => '');
echo form_open('settings/manage/'.$op, $attributes); ?>
 <p>
        <label for="appemail">Administrative email <span class="required">*</span></label>
        <?php echo tooltip('tooltip_application_email','',true) ; //echo form_error('appemail'); ?>
        <input id="appemail" type="text" name="appemail" maxlength="50" value="<?php echo $appemail; ?>"  />
</p>

<p>
   <label for="authentication">Authentication method <span class="required">*</span></label>
        <?php  // echo form_error('mode'); ?>
                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                     <input id="mode1" name="mode" type="radio" class="" value="database" <?php echo (isset($database))?$database:$this->form_validation->set_radio('mode', 'database') ; ?> checked="checked"/>
        		<label for="mode" class="">Internal</label>
                        
                        <input id="mode2" name="mode" type="radio" class="" value="ldap" <?php echo (isset($ldap))?$ldap:$this->form_validation->set_radio('mode', 'ldap'); ?> />
        		<label for="mode" class="">LDAP</label>
                        
                        <input id="mode3" name="mode" type="radio" class="" value="active_directory" <?php echo (isset($active_directory))?$active_directory:$this->form_validation->set_radio('mode', 'active_directory') ; ?> />
        		<label for="mode" class="">Active Directory</label>

                       
</p>

<fieldset id="ldapsettings">
    <legend></legend>
<p>
        <label for="host">LDAP host <span class="required">*</span></label>
        <?php  echo tooltip('tooltip_host_name','',true) ;  //echo form_error('host'); ?>
        <input id="host" type="text" name="host" maxlength="50" value="<?php echo $host; ?>"  />
</p>

<p>
        <label for="base_dn">Base dn <span class="required">*</span></label>
        <?php echo tooltip('tooltip_base_dn','',true) ; // echo form_error('base_dn'); ?>
        <input id="base_dn" type="text" name="base_dn" maxlength="50" value="<?php echo $base_dn; ?>"  />
</p>

<p>
        <label for="login_attribute">Login attribute <span class="required ldaprelated">*</span></label>
        <?php echo tooltip('tooltip_login_aloginrelatedttr','',true) ;// echo form_error('login_attribute'); ?>
        <input id="login_attribute" type="text" name="login_attribute"  value="<?php echo $login_attribute; ?>"  />
</p>

<p>
        <label for="users_directory">User directory <span class="required ldaprelated">*</span></label>
        <?php echo tooltip('tooltip_user_dir','',true) ;// echo form_error('login_attribute'); ?>
        <input id="users_directory" type="text" name="users_directory"  value="<?php echo $users_directory ?>"  />
        <span id="effective_dir"></span>
</p>

<p id="member_attribute_related">
        <label for="member_attribute">Member attribute <span class="required"></span></label>
        <?php echo tooltip('tooltip_member_attr','',true) ;//echo form_error('member_attribute'); ?>
        <input id="member_attribute" type="text" name="member_attribute"  value="<?php echo $member_attribute ?>"  />
</p>


<p class="adrelated">
        <label for="active_directory_domain">Active directory domain <span class="required">*</span></label>
        <?php echo tooltip('tooltip_ad_domain_name','',true) ;// echo form_error('active_directory_domain'); ?>
        <input id="active_directory_domain" type="text" name="active_directory_domain"  value="<?php echo $active_directory_domain  ?>"  />
</p>

<p>
   <label for="encryption">Encryption<span class="required"></span></label>
        <?php  // echo form_error('mode'); ?>
      
                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                     <input id="sec1" name="encryption" type="radio" class="" value="plain" <?php echo (isset($plain))?$plain:$this->form_validation->set_radio('encryption', 'plain') ; ?>/>
                     <label for="encryption" class="">None</label>

                   <input id="sec2" name="encryption" type="radio" class="" value="ssl" <?php echo (isset($ssl))?$ssl:$this->form_validation->set_radio('encryption', 'ssl'); ?> />
        	  <label for="encryption" class="">SSL</label>

                   <input id="sec3" name="encryption" type="radio" class="" value="start_tls" <?php echo (isset($start_tls))?$start_tls:$this->form_validation->set_radio('encryption', 'start_tls') ; ?> />
                  <label for="encryption" class="">STARTTLS</label>
</p>

<p><label></label> <a class="btn" id="testsettings" href="<?php echo site_url('settings/ldaptest')?>">Test settings</a></p>
</fieldset>
<p>
    
    <label for="fall back for">Fall-back group ( if authentication server down)<span class="required"></span></label>
    <?php echo tooltip('tooltip_fall_back','',true) ;// echo form_error('active_directory_domain'); ?>
    <?php echo form_dropdown('fall_back_for', $groups, $fall_back_for?$fall_back_for:'select');?>
</p>

 <?php if(isset( $groupsacc)){?>
<p>
    <label for="fall back for">Administrative group<span class="required"></span></label>
   <?php echo tooltip('tooltip_admin_grp','',true) ;// echo form_error('active_directory_domain'); ?>
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
                $('.adrelated').hide();
                $('#member_attribute_related').show();
                $('.ldaprelated').show();
                
            }
            else if($("input[@name='mode']:checked").val() == 'active_directory'){
                $('#ldapsettings').show()
                $('.adrelated').show();
                $('#member_attribute_related').hide();
                $('.ldaprelated').hide();
            }
            if($("#base_dn").val()!='')
                {
                set_bind_dn();
                }
     }
     
     function set_bind_dn(){
            if($('#users_directory').val()!=''){
              $('#effective_dir').text('bind dn: '+$("#users_directory").val()+','+$("#base_dn").val())
               }else{
              $('#effective_dir').text('bind dn: '+$("#base_dn").val())
                }  
     }
     
     
     $("input[name='users_directory']").keyup(function(){
        set_bind_dn();
     });
     
     $("input[name='base_dn']").keyup(function(){
        set_bind_dn();
     });

      $('#testsettings').bind('click',function(event){
          event.preventDefault();
          if($("input:radio[name=mode]:checked").val()=='active_directory'){
              
          }
          $(this).ajaxyDialog({title:'LDAP Test',clickData:{
             'mode':$("input:radio[name=mode]:checked").val(),
             'host':$("#host").val(),
             'basedn':$("#base_dn").val(),
             'login_attr':$("#login_attribute").val(),
             'user_dir':$("#users_directory").val(),
             'member_attr':$("#member_attribute").val(),
             'addomain':$("#active_directory_domain").val(),
             'encryption':$("input:radio[name=encryption]:checked").val()
         }}).ajaxyDialog('open');
       
      });
      
      $("input[name='mode']").change(function(){
      settings_toggle();
      $("select[name='admin_group']").find('option') .remove().end()
     });
    <?php if ($this->setting_lib->get_tooltips_status()) { ?>   
    $('span.hint').each(function() // Find all inputs with formtip spans next to them
    {
        $(this).next('input, select, textarea').eq(0).qtip( // Stick a tooltip on the spans related input
        {
            content: {
                text:$(this).html() // Use the spans HTML for the tooltip content
            },
            position: {
                    my: 'left center',
                    at: 'right center',
                    target:$(this).next('input,select').eq(0),
                    viewport:$(window)
            },
            show:{ event: 'foucs mouseenter'},
            hide: { event: 'mouseout'},
            style:{
              tip: false
            }
        });
    }); 
   <?php } ?>

});
</script>
