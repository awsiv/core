<div class="outerdiv grid_12">
          		
    <div class="innerdiv">

<div id="admin_content">
<h1>Your profile</h1>
<?php if (empty($user)) { ?>
<p>We don't have any information about you.</p>
<?php } else { ?>

        <table cellspasing="5" style="width:30%; float: left">
            <tr>
                <td>User name</td>
                <td><?php echo $user['username']?></td>
            </tr>
           <?php if ($this->session->userdata('mode') == 'internal') { ?>
            <tr>
                <td>Email</td>
                <td><?php echo $user->email ?></td>
            </tr>
            <tr>
                <td>Change password</td>
                <td>
                    <?php  echo anchor("auth/change_password/" . $user['username'], ' ', array('class' => 'changepassword', 'title' => 'change password')); ?>
                </td>
            </tr>
            <?php } ?>
            <tr>
                <td colspan="2">Role memberships</td>
            </tr>
            <tr>
                <td colspan="2"><div class="DragNDrop assigned dialog_box_style ui-dialog"  style="position: relative">
                        <div class="itemwrapper assigneditems">

                            <?php if (!empty($user['roles'])) { 
                                $i = 1;
                                $first = true;
                                $fist_item_class = ' class="first"';


                                echo '<ul id="roles" class="roleslist itemlist assignedList">';

                                foreach ($user['roles'] as $item) {
                                    echo '<li ' . ($first === TRUE ? $fist_item_class : '') . ' >';
                                    echo $item;
                                    echo "</li>";
                                    $i++;
                                    $first = FALSE;
                                }

                                echo '</ul>';
                            } else { ?>
                                <ul id="roles" class="roleslist itemlist assignedList empty_list"></ul>
                                <div class="empty_list_warning">No items assigned</div>
                            <?php } ?>
                        </div>
                    </div>            
                </td>
            </tr>            
        </table>
        <div id="passwd_form" style="width: 45%; float: left; margin-left: 20px; display:none"></div>
         <div class="clear"></div>
<?php } ?>
    </div>
</div>   
    </div>
<script type="text/javascript">
        // common options for ajax requests
        $.ajaxSetup({
            beforeSend: function() {
                $.blockUI({ css:{
                        border: 'none',
                        padding: '15px',
                        backgroundColor: '#000',
                        '-webkit-border-radius': '10px',
                        '-moz-border-radius': '10px',
                        opacity: .5,
                        color: '#fff'
                    },
                    message: '<h1><img src="<?php echo get_imagedir() ?>ajax_loader2.gif" />Please wait...</h1>'
                });
            },
            complete: function(){
                $('.blockUI').hide();   
                $(document).unblock();
                $.unblockUI();
            },
            success: function() {
                $('.blockUI').hide();   
                $(document).unblock();
                $.unblockUI();
            }
        });   
        
        $(document).ready(function() {    
            var options = {
            target:  '#passwd_form'  // target element(s) to be updated with server response
        };    

        $(document).ajaxStop(function(){
            setTimeout(function() {
                $("#infoMessage, #formInfoMessage").fadeOut(500)
            }, 10000);
        });

    //loading the change password in admin_content
    $('a.changepassword').live('click',function(event){
        event.preventDefault();
        var path=$(this).attr('href');
        $('#passwd_form').toggle(0, function() {
            $("#passwd_form").load(path);
            $("#passwd_form").css('border', '1px solid #ccc');
        });

    });

    //submitting the form ajaxically to the page in form action and loading the result in admin_content
    $('#change_password').live('submit',function(event){
        event.preventDefault();
        $(this).ajaxSubmit(options);
    });
});
</script>    
