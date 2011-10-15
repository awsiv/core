<div class="outerdiv">
<div class=" innerdiv ">
    <div class="panelhead">Mission Portal settings</div>
    <div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
   <?php // Change the css classes to suit your needs

$attributes = array('class' => '', 'id' => 'mpsettings');
echo form_open('settings/manage/'.$op, $attributes); ?>
 <p>
        <label for="appemail">Administrative email <span class="required">*</span></label>
        <?php echo tooltip('tooltip_application_email','',true) ; //echo form_error('appemail'); ?>
        <input id="appemail" type="text" name="appemail" maxlength="50" value="<?php echo $appemail; ?>"  />
</p>

 
<p id='admingrpsec'>
    <label for="admin_group">Administrative group<span class="required"></span></label>
   <?php if(isset( $groupsacc)){?>
   <?php echo tooltip('tooltip_admin_grp','',true) ;// echo form_error('active_directory_domain'); ?>
   <?php  echo form_dropdown('admin_group', $groupsacc, $admin_group?$admin_group:'select');?>
   <?php }?>
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


<div id="userdirs">
<p>
        <label for="users_directory">User directory <span class="required ldaprelated">*</span></label>
        <?php echo tooltip('tooltip_user_dir','',true) ;// echo form_error('login_attribute'); ?>
        <input id="users_directory" type="text" name="users_directory[]"  value="<?php echo $users_directory ?>" class="usrdir"/>
        <?php if(!isset($user_dirs)){ ?>
        <button id="addmore">add</button>
        <?php } ?> 
        <span id="effective_dir"></span>
</p>
 <?php 
       if(isset($user_dirs)){
         $number_of_dirs=count($user_dirs);
         $i=1;
         foreach($user_dirs as $dirs) {
              echo "<p>";
              echo form_label();
              echo form_input($dirs);
              $data = array(
                    'name' => 'button',
                    'content' => 'remove',
                    'type'=>'button',
                    'class'=>'rmbtns'
                    );
              echo form_button($data);
               if($i==$number_of_dirs){
                  echo form_button(array('id'=>'addmore','content'=>'add')); 
              }
              echo "</p>";
              $i++;
            }
       }
   ?>
</div>

<p class="member_attribute_related">
        <label for="member_attribute">Member attribute (secondary)<span class="required"></span></label>
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
     var mode= $("input[name='mode']:checked").val();
     var elem=$("select[name='admin_group']").clone();
     var grpcontainer=$('#admingrpsec');
      settings_toggle();
        function settings_toggle(){
            if ($("input[@name='mode']:checked").val() == 'database')
            {
                $('#ldapsettings').hide();
                if(mode!='database'){
                $.get("<?php echo site_url('settings/get_native_groups')?>", function(result){
                     grpcontainer.append(result);
                  });
                }
            }
            else if ($("input[@name='mode']:checked").val() == 'ldap')
            {
                $('#ldapsettings').show();
                $('.adrelated').hide();
                $('.member_attribute_related').show();
                $('.ldaprelated').show();
                $("#addmore").show();
                
            }
            else if($("input[@name='mode']:checked").val() == 'active_directory'){
                $('#ldapsettings').show()
                $('.adrelated').show();
                $('.member_attribute_related').hide();
                $('.ldaprelated').hide();
                $("#addmore").hide();
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
     
     
     $("input[name='users_directory[]']").keyup(function(){
        set_bind_dn();
     });
     
     $("input[name='base_dn']").keyup(function(){
        set_bind_dn();
     });
     
    $("#addmore").bind('click', function(event){
        event.preventDefault();
        var num=$('.usrdir').length;
        var newElem = $('#users_directory').clone().attr('id', 'users_directory_'+num).val('');
        var rmNewElem=$('<button>').attr('class','rmbtns').text('remove');
        var container=$("<p>").append('<label>').append(newElem).append(rmNewElem).append($(this));
        $('#userdirs').append(container);    
    });
    
    $(".rmbtns").live('click',function(event){
       var addmorebtn=$('#addmore');
       if($(this).parent().prev().find('#effective_dir').length>0){
           $('#effective_dir').before(addmorebtn);
       }else{
          $(this).parent().prev().append(addmorebtn); 
       }
       $(this).parent().remove();
    });

      $('#testsettings').bind('click',function(event){
          event.preventDefault();
          var no_dirs=new Array();
          $("#userdirs").find("input:text[name=users_directory[]]").each(function(index, value) {
                  no_dirs.push($(this).val());
          })
           
          $(this).ajaxyDialog({title:'LDAP Test',clickData:{
             'mode':$("input:radio[name=mode]:checked").val(),
             'host':$("#host").val(),
             'basedn':$("#base_dn").val(),
             'login_attr':$("#login_attribute").val(),
             'user_dir':no_dirs.join(';'),
             'member_attr':$("#member_attribute").val(),
             'addomain':$("#active_directory_domain").val(),
             'encryption':$("input:radio[name=encryption]:checked").val()
         },
         submitDatatype:'json',
         change:function(response){
             $('#infoMessage').html(response.message);
              $("select[name='admin_group']").remove();
             grpcontainer.append(response.groups);
         }
     }).ajaxyDialog('open');
       
      });
      
      $("input[name='mode']").change(function(){  
      settings_toggle();
      var selbox=$("select[name='admin_group']");
      if($(this).val()==mode && selbox.length==0){
          grpcontainer.append(elem)
          //$('#grpmsg').remove();
      }else{
          selbox.remove();  
      }
          
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
