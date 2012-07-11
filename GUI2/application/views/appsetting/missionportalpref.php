<div class="outerdiv">
    <div class=" innerdiv ">
        <div class="panelhead">Mission Portal settings</div>
        <div id="infoMessage"><?php echo $message; ?>
            <?php
            if (($this->session->flashdata('message')))
            {
                $flashMessage = $this->session->flashdata('message');
                $content = $flashMessage['content'];
                $content_class = $flashMessage['type'];
                ?>
                <p class="<?php echo $content_class; ?>"><?php echo $content; ?></p>
            <?php } ?>
        </div>
        <div class="form">
            <?php
            // Change the css classes to suit your needs

            $attributes = array('class' => '', 'id' => 'mpsettings');
            echo form_open('settings/show', $attributes);
            ?>
            <p>
                <label for="appemail">Administrative email <span class="required">*</span></label>
                <?php echo tooltip('tooltip_application_email', '', true); //echo form_error('appemail');  ?>
                <input id="appemail" type="text" name="appemail" maxlength="50" value="<?php echo $appemail; ?>"  />
            </p>


            <p id='admingrpsec'>
                <label for="admin_role">Administrative role<span class="required"></span></label>
                <?php
                if (isset($roles))
                {
                    echo tooltip('tooltip_admin_grp', '', true); // echo form_error('active_directory_domain');
                    echo form_dropdown('admin_role', $roles, isset($admin_role) ? $admin_role : 'select');
                }
                ?>

                <?php
                if (isset($selected_role))
                {
                    echo form_label($selected_role, '', array('id' => 'selected_grplbl'));
                }
                ?>
 <!--<a class="btn testldap" id="getgrpsbtn" href="<?php echo site_url('settings/ldaptest') ?>">Get roles</a>-->
            </p>



            <p>
                <label for="authentication">Authentication method <span class="required">*</span></label>
                <?php // echo form_error('mode');  ?>
                <?php // Change or Add the radio values/labels/css classes to suit your needs  ?>
                <input id="mode1" name="mode" type="radio" class="" value="database" <?php echo (isset($database)) ? $database : $this->form_validation->set_radio('mode', 'database'); ?> checked="checked"/>
                <label for="mode" class="">Internal</label>

                <input id="mode2" name="mode" type="radio" class="" value="ldap" <?php echo (isset($ldap)) ? $ldap : $this->form_validation->set_radio('mode', 'ldap'); ?> />
                <label for="mode" class="">LDAP</label>

                <input id="mode3" name="mode" type="radio" class="" value="active_directory" <?php echo (isset($active_directory)) ? $active_directory : $this->form_validation->set_radio('mode', 'active_directory'); ?> />
                <label for="mode" class="">Active Directory</label>

            </p>

            <fieldset id="ldapsettings">
                <legend></legend>
                <p>
                    <label for="host">LDAP host <span class="required">*</span></label>
                    <?php echo tooltip('tooltip_host_name', '', true);  //echo form_error('host');  ?>
                    <input id="host" type="text" name="host" maxlength="50" value="<?php echo $host; ?>"  />
                </p>

                <p>
                    <label for="base_dn">Base dn <span class="required">*</span></label>
                    <?php echo tooltip('tooltip_base_dn', '', true); // echo form_error('base_dn');  ?>
                    <input id="base_dn" type="text" name="base_dn" maxlength="50" value="<?php echo $base_dn; ?>"  />
                </p>

                <p>
                    <label for="login_attribute">Login attribute <span class="required ldaprelated">*</span></label>
                    <?php echo tooltip('tooltip_login_aloginrelatedttr', '', true); // echo form_error('login_attribute'); ?>
                    <input id="login_attribute" type="text" name="login_attribute"  value="<?php echo $login_attribute; ?>"  />
                </p>


                <div id="userdirs">
                    <p>
                        <label for="users_directory">User directory <span class="required ldaprelated">*</span></label>
                        <?php echo tooltip('tooltip_user_dir', '', true); // echo form_error('login_attribute'); ?>
                        <?php
                        if (empty($user_dirs)) {    ?>
                            <input id="users_directory" type="text" name="user_directory[]" class="usrdir"/>
                            <button id="addmore">add</button>
                            <?php  }   else  {
                            $number_of_dirs = count($user_dirs);
                            foreach ($user_dirs as $i => $dirs)
                            {
                                echo "<p>";
                                echo form_label();
                                $input = array(
                                    'name' => 'user_directory[]',
                                    'id' => 'user_directory-' . $i,
                                    'value' => $dirs,
                                    'class' => 'usrdir');
                                $i++;
                                echo form_input($input);
                                $data = array(
                                    'name' => 'button',
                                    'content' => 'remove',
                                    'type' => 'button',
                                    'class' => 'rmbtns'
                                );
                                echo form_button($data);
                                if ($i == $number_of_dirs)
                                {
                                    echo form_button(array('id' => 'addmore', 'content' => 'add'));
                                }
                                echo "</p>";
                            }
                        }
                        ?>

                    </p>
                </div>
                <p class="adrelated">
                    <label for="active_directory_domain">Active directory domain <span class="required">*</span></label>
                    <?php echo tooltip('tooltip_ad_domain_name', '', true); // echo form_error('active_directory_domain');      ?>
                    <input id="active_directory_domain" type="text" name="active_directory_domain"  value="<?php echo $active_directory_domain ?>"  />
                </p>

                <p>
                    <label for="encryption">Encryption<span class="required"></span></label>
                    <?php // echo form_error('mode');     ?>

                    <?php // Change or Add the radio values/labels/css classes to suit your needs     ?>
                    <input id="sec1" name="encryption" type="radio" class="" value="plain" <?php echo (isset($plain)) ? $plain : $this->form_validation->set_radio('encryption', 'plain'); ?>/>
                    <label for="encryption" class="">None</label>

                    <input id="sec2" name="encryption" type="radio" class="" value="ssl" <?php echo (isset($ssl)) ? $ssl : $this->form_validation->set_radio('encryption', 'ssl'); ?> />
                    <label for="encryption" class="">SSL</label>

                    <input id="sec3" name="encryption" type="radio" class="" value="start_tls" <?php echo (isset($start_tls)) ? $start_tls : $this->form_validation->set_radio('encryption', 'start_tls'); ?> />
                    <label for="encryption" class="">STARTTLS</label>
                </p>
                <p>
                    <label for="external_admin_username">External admin user name<span class="required">*</span></label>
                    <?php echo tooltip('tooltip_external_admin_username', '', true); ?>
                    <input id="external_admin_username" type="text" name="external_admin_username" value="<?php echo (isset($external_admin_username) ? $external_admin_username : '') ?>" />
                </p>

                <p><label></label> <a class="btn testldap" id="testsettings" href="<?php echo site_url('settings/ldaptest') ?>">Test settings</a></p>
            </fieldset>
            <p>
                <label for="fall back for">Fall-back role ( if authentication server down)<span class="required"></span></label>
                <?php echo tooltip('tooltip_fall_back', '', true); // echo form_error('active_directory_domain');  ?>
                <?php echo form_dropdown('fall_back_for', $roles, $fall_back_for ? $fall_back_for : 'select'); ?>
            </p>
            <p>
                <label for="rbac">Role based access control <span class="required">*</span></label>
                <?php
                $elem = array(
                    'name' => 'rbac',
                    'id' => 'rbac_on',
                    'value' => 'true',
                    'checked' => (isset($rbac) && $rbac ? TRUE : '')
                );
                echo form_radio($elem);
                ?>
                <label for="mode" class="">On</label>
                <?php
                $elem = array(
                    'name' => 'rbac',
                    'id' => 'rbac_off',
                    'value' => 'false',
                    'checked' => (isset($rbac) && !$rbac ? TRUE : '')
                );
                echo form_radio($elem);
                ?>

                <label for="mode" class="">Off</label>
            </p>

            <p>
                <label for="bluehost_threshold_global">Unreachable host threshold (minutes) <span class="required">*</span></label>
                <?php echo tooltip('tooltip_bluehost_threshold', '', true); ?>
                <input id="bluehost_threshold_global" type="text" name="bluehost_threshold_global" maxlength="5" value="<?php echo $bluehost_threshold_global ?>"  /> <?php
                if (!empty($bluehost_threshold_global))
                {
                    echo "(" . time_diff_conv($bluehost_threshold_global * 60) . ")";
                }
                ?>

            </p>

            <p  id="btnholder">
                <?php echo form_submit(array('name' => 'submit', 'class' => 'btn', 'value' => 'Submit')); ?>
            </p>

            <?php echo form_close(); ?>

        </div>
    </div>
</div>
<script type="text/javascript">
    $(document).ready(function(){
        $('#ldapsettings').hide();
        var mode= $("input[name='mode']:checked").val();
        var elem=$("select[name='admin_role']").clone();
        var grplbl=$('#selected_grplbl').clone();
        //var getgrpbtn=$('#getgrpsbtn').clone();
        var grpcontainer=$('#admingrpsec');
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
                //$('.member_attribute_related').show();
                $('.ldaprelated').show();
                $("#addmore").show();
                //$('#getgrpsbtn').show()

            }
            else if($("input[@name='mode']:checked").val() == 'active_directory'){
                $('#ldapsettings').show()
                $('.adrelated').show();
                //$('.member_attribute_related').hide();
                $('.ldaprelated').hide();
                $("#addmore").hide();
                //$('#getgrpsbtn').show()
            }
        }

        $("#addmore").bind('click', function(event){
            event.preventDefault();
            var num=$('.usrdir').length;
            var $userDirTemplate = $('<input type="text" name="user_directory[]" class="usrdir">');

            var newElem = $userDirTemplate.attr('id', 'users_directory_'+num).val('');
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

        $('.testldap').bind('click',function(event){
            event.preventDefault();
            var no_dirs=new Array();
            $("#userdirs").find("input:text[name=users_directory[]]").each(function(index, value) {
                no_dirs.push($(this).val());
            })
            var dlgtitle='LDAP Test';
            // if($(this).attr('id')=='getgrpsbtn'){
            //    dlgtitle='Get roles' ;
            // }

            $(this).ajaxyDialog({title:dlgtitle,clickData:{
                    'mode':$("input:radio[name=mode]:checked").val(),
                    'host':$("#host").val(),
                    'basedn':$("#base_dn").val(),
                    'login_attr':$("#login_attribute").val(),
                    'user_dir':no_dirs.join(';'),
                    //'member_attr':$("#member_attribute").val(),
                    'addomain':$("#active_directory_domain").val(),
                    'encryption':$("input:radio[name=encryption]:checked").val()
                },
                submitDatatype:'json',
                change:function(response){
                    $('#infoMessage').html(response.message);
                    // $("select[name='admin_role']").remove();
                    // $('#selected_grplbl').remove();
                    // grpcontainer.append(response.roles);
                    // $('#getgrpsbtn').before(response.roles);
                }

            }).ajaxyDialog('open');

        });

        $("input[name='mode']").change(function(){
            settings_toggle();
            /*var selbox=$("select[name='admin_role']");
      if($(this).val()==mode){
          selbox.remove();
          $('#selected_grplbl').remove();
          $('#getgrpsbtn').before(elem).before(grplbl);
      }
     else{
          selbox.remove();
          $('#selected_grplbl').remove();
      }*/

        });
<?php
if ($this->setting_lib->get_tooltips_status())
{
    ?>
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
                        hide: { event: 'mouseout'}
                    });
                });
<?php } ?>

    });
</script>
