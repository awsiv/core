   <style> 
    <!--
    .roles_wrapper {
       float: left;
       width: 98%
    }
    .role_list {
         float: left; 
         width: 40%; 
         overflow: auto;
    }
    .role_list ul {
         height: 400px;
         border: 1px solid #D4D5D0;
         padding: 5px 10px;
         overflow: auto;
    }
    .role_list ul li {
        list-style: none;
        border-top: 1px solid #D4D5D0;
        padding: 10px 0;
    }
    .role_list ul li.first {
        border: none !important;
    }
    
    .switchers {
        float: left;
        width: 7%;
        text-align: center;
        margin: 0 20px;
        padding-top: 150px;
    }
    .selected_item {
        background: #eee;
    }
    -->
   </style>
<div class="outerdiv grid_12">

    <div class="innerdiv">
<!--<p><?php echo "Logged in: $username"; ?>&nbsp;<a href="<?php echo site_url('auth/logout'); ?>">Logout</a></p>-->
        <ul class="admin_menu">
            <li class="current"><a href="<?php echo site_url('auth/index'); ?>">Users</a></li>
            <li><a href="<?php echo site_url('auth/manage_role'); ?>">Roles</a></li>
            <!--<li>Settings</li>-->
        </ul>
        <div class="holder">
            <div id="error_status"></div>
            <div id="admin_content" class="tables">
                <?php include 'user_list.php'; ?>
            </div>
        </div>
    </div>
</div>
<div id="confirmation" title="Are you sure"><span>Do you want to continue deleting..</span></div>
<script src="<?php echo get_scriptdir() ?>jquery.form.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir() ?>jquery.blockUI.js" type="text/javascript"></script>
<script type="text/javascript">
    $(document).ready(function() {
        var options = {
            target:  '#admin_content'  // target element(s) to be updated with server response
        };

        //loading the create user page from server to add the user
        $('#add_user').live('click',function(event) {
            event.preventDefault();
            $("#error_status").html('');            
            var path=$(this).attr('href');
            $("#admin_content").slideUp().load(path).slideDown();;
        });

        //submitting the create user form
        $('#create_user').live('submit',function(event) {
            event.preventDefault();
            $("#error_status").html('');            
            $(this).ajaxSubmit(options)
        });

        //load the result from the server after delete page is called

        $('a.delete').live('click',function(event){
            event.preventDefault();
            $("#error_status").html('');
            var path=$(this).attr('href');
            $confirmation.data('path',path).dialog("open");
        });


        var $confirmation = $('#confirmation').dialog({
            autoOpen: false,
            modal: true,
            resizable: false,
            buttons: {
                'Cancel':function() {
                    $(this).dialog('close');
                },
                'Confirm': function() {
                    $("#admin_content").load($(this).data('path'), 
                    function(responseText, textStatus, XMLHttpRequest) {
                        switch (XMLHttpRequest.status) {
                            case 200: break;
                            case 401:
                            case 404:
                                $('#error_status').html('<p class="error">' + responseText + '</p>');
                                break;
                            default:
                                $('#error_status').html('<p class="error">' + '<?php echo $this->lang->line('500_error'); ?>' +  '</p>');
                                break;
                            }
                        }
                    );
                        $(this).dialog('close');
                    }
                },
                open: function() {
                    $(this).parent().find('.ui-dialog-buttonpane').find('button:last').focus()
                }
            });

            //loading the edit page in admin_content of the admin area
            $('a.edit').live('click',function(event){
                event.preventDefault();
                $("#error_status").html('');
                var path=$(this).attr('href');
                $("#admin_content").load(path,function(res){
                    $(this).html(res);
                });
            });

            //loading the change password in admin_content
            $('a.changepassword').live('click',function(event){
                event.preventDefault();
                $("#error_status").html('');
                var path=$(this).attr('href');
                $("#admin_content").load(path);
            });
            //submitting the form ajaxically to the page in form action and loading the result in admin_content
            $('#edit_user').live('submit',function(event)
            {
                event.preventDefault();
                $(this).ajaxSubmit(
                {
                    target:'#admin_content',beforeSubmit: function(arr, $form, options)
                    {
                        $.blockUI
                        (
                        { css:
                            {
                                border: 'none',
                                padding: '15px',
                                backgroundColor: '#000',
                                '-webkit-border-radius': '10px',
                                '-moz-border-radius': '10px',
                                opacity: .5,
                                color: '#fff'
                            },
                            message: '<h1><img src="<?php echo get_imagedir() ?>ajax_loader2.gif" />Please wait...</h1>'
                        }
                        );
                    },
                    success:function(responseText, statusText, xhr, $form){
                        $(document).unblock();
                    }      
                });
            });

            //submitting the form ajaxically to the page in form action and loading the result in admin_content
            $('#change_password').live('submit',function(event){
                event.preventDefault();
                $(this).ajaxSubmit(options);
            });

            //loading all the pages in admin content after clicking the items in admin index menu
            $('.admin_menu li a').click(function(event){
                event.preventDefault();
                var path=$(this).attr('href');
                $("#error_status").html('');
                $("#admin_content").load(path);
                $(this).parent().addClass('current').siblings().removeClass('current');
            });

            //loading the  role create page in the  admin area ajaxcially
            $('#add_role').live('click',function(event){
                event.preventDefault();
                $("#error_status").html('');
                var path=$(this).attr('href');
                $("#admin_content").slideUp().load(path).slideDown();
            });

            //create a new role form the page loaded
            $('#create_role').live('submit',function(event){
                event.preventDefault();
                $("#error_status").html('');
                $(this).ajaxSubmit(options);
            });

            $('#Create_role').live('submit',function(event){
                event.preventDefault();
                $("#error_status").html('');
                $(this).ajaxSubmit(options);
            });


            $('#Update_role').live('submit',function(event){
                event.preventDefault();
                $(this).ajaxSubmit(options);
            });
  
            $(document).ajaxStop(function(){
                setTimeout(function() {
                    $("#infoMessage").fadeOut(500)
                }, 10000);
            });

            $('.activate').live('click',function(event){
                event.preventDefault();
                var path=$(this).attr('href');
                $("#admin_content").load(path);
            });

            $('.inactivate').live('click',function(event){
                event.preventDefault();
                var path=$(this).attr('href');
                $("#admin_content").load(path);
            });
 
             $('#deactivate_user').live('submit',function(event) {
                event.preventDefault();
                $(this).ajaxSubmit(options)
            });
        });

// roles 
    $('#all_roles li, #user_roles li').live('click',function(event) {
        //var itemid = $(this).parent().attr('itemid');
        var itemid = $(this).attr('itemid');
        $("#li_item" + itemid).toggleClass("selected_item");
       // $(this).find('input:checkbox').attr('checked', 'checked');            
    });
    
    
    //moving roles
   $("#move_left").live('click',function(event) {
        $('#all_roles .selected_item').each(function() {
            var item_id = $(this).attr('itemid');
            $('#li_itemid' + item_id).each(function() {this.checked = false;});

            var item_clone = $(this).clone(true);
            $(item_clone).removeClass('selected_item');
            $(item_clone).addClass('moved');
            $(item_clone).find('input:checkbox').attr('checked', 'checked');  
            $(item_clone).find('input:checkbox').attr('disabled', false);
            $('#user_roles').append(item_clone);
            $(this).remove();
	});
    });
    $("#move_right").live('click',function(event) {
        $('#user_roles .selected_item').each(function() {
            var item_id = $(this).attr('itemid');
       
            $('#li_itemid' + item_id).each(function() {this.checked = false;});
            var item_clone = $(this).clone(true);
            $(item_clone).removeClass('selected_item');
            $(item_clone).addClass('moved');
            $(item_clone).find('input:checkbox').attr('checked', false);
            $(item_clone).find('input:checkbox').attr('disabled', true);
            $('#all_roles').append(item_clone);
            $(this).remove();
        });
    });
  
</script>