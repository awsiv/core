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
<div id="confirmation" title="Are you sure"><span></span></div>
<script src="<?php echo get_scriptdir() ?>jquery.form.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir() ?>jquery.blockUI.js" type="text/javascript"></script>
<script type="text/javascript">
    $(document).ready(function() {
        var options = {
            target:  '#admin_content'  // target element(s) to be updated with server response
        };
        
        // buttons wrapper, just set label, action (if needed), and dialog object
        function generateCloseBtn(label,dialogObj) {
           var btn = [{
                        text: label,
                        click: function() {
                                dialogObj.dialog('close');
                            }
           }];
           return btn;
        }
        
        function generateLoadBtn(label, link, dialogObj) {
            var btn = [{
                        text: label,
                        click: function()
                            {
                                $("#admin_content").load(link, 
                                function(responseText, textStatus, XMLHttpRequest) {
                                    switch (XMLHttpRequest.status) {
                                        case 200: break;
                                        case 401:
                                        case 404:
                                            $('#error_status').html('<p class="error">' + responseText + '</p>');
                                            break;
                                        default:
                                            $('#error_status').html('<p class="error">' + '<?php //echo $this->lang->line('500_error'); ?>' +  '</p>');
                                            break;
                                        }
                                    }
                                );
                                dialogObj.dialog('close');
                            }
             }];
             return btn;
         }
            
        function generateSubmitBtn(label, form, dialogObj) {
            var btn = [{
                        text: label,
                        click: function()
                            {
                                form.ajaxSubmit(options);
                                dialogObj.dialog('close');
                            }
                }];
            return btn;
        }
        //dialog object                
        var $confirmation = $('#confirmation').dialog({
            autoOpen: false,
            modal: true,
            resizable: false,
            buttons: [],
                open: function() 
                    {
                        $(this).parent().find('.ui-dialog-buttonpane').find('button:last').focus()
                    }
        });        

        //loading the create user page from server to add the user
        $('#add_user').live('click',function(event) {
            event.preventDefault();
            $("#error_status").html('');       
  
            var path = $(this).attr('href');
            attach_edit_form(this, $(this).attr('form'));

        });

        //submitting the create user form
        $('#create_user').live('submit',function(event) {
            event.preventDefault();
            $("#error_status").html('');    
            options.success = function () {
                 bindSortable('roleslist', new Array('roles'));
                } 
            $(this).ajaxSubmit(options);
        });

        //load the result from the server after delete page is called
        $('a.delete').live('click',function(event){
            event.preventDefault();
            $("#error_status").html('');
            var path = $(this).attr('href');
            $('#confirmation span').text('Do you want to continue deleting..');

            // create buttons - cancel and confirm
            var cancelBtn  = generateCloseBtn('Cancel', $confirmation);
            var confirmBtn = generateLoadBtn('Confirm', path, $confirmation);

            $confirmation.dialog("option", "buttons", cancelBtn.concat(confirmBtn) );
            $confirmation.dialog("open");
        });

        //loading the edit page in admin_content of the admin area
        $('a.edit').live('click',function(event){
            event.preventDefault();
            attach_edit_form(this, $(this).attr('form'));
        });

function attach_edit_form(elem, form) {
    var parent = $(elem).parent().parent(); //get parent element   

    //delete element
    if($("#edit_form_wrapper").length) {
        $("#edit_form_wrapper").remove();
    }

    //create element with the same type as parent
    $('<' + $(parent).get(0).tagName + ' id="edit_form_wrapper"><td colspan=3 style="background:#ccc"><div id="edit_form"></div></td></' + $(parent).get(0).tagName +'>').insertAfter(parent);  
    $("#error_status").html('');
    var path = $(elem).attr('href');
    load_edit_form(path, form);
}

function load_edit_form(path, form) {
    $("#edit_form").load(path,function(res){
        $("#edit_form").html(res).slideDown('slow',  function() {
 
            if (form == 'role')
            {
                addFinders();
            }  
            else
            {
                bindSortable('roleslist', new Array('roles'))
            }
        });
    });
}


function addFinders() {
            var genericOption = {
                baseUrl: '<?php echo site_url() ?>',
                addSearchBar: true,
                addAlphabetFilter: true,
                itemId: 'classList',
                placeholderId: 'classList_wrapper',
                sortable: true,
                sortableConnectionClass: 'classlist',
                sortableDestinations: new Array('crxi', 'crxx')
            };

            $('#classList').classfinderbox(genericOption);

            var genericOption = {
                baseUrl: '<?php echo site_url() ?>',
                addSearchBar: false,
                addAlphabetFilter: false,
                itemId: 'bundlessList',
                placeholderId: 'bundlessList_wrapper',
                sortable: true,
                sortableConnectionClass: 'bundlelist',
                sortableDestinations: new Array('brxi', 'brxx')                    
            };
        
            $('#bundlessList').classfinderbox(genericOption);
}

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
                    target: options.target, beforeSubmit: function(arr, $form, options)
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
                        bindSortable('roleslist', new Array('roles'));
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
                attach_edit_form(this, 'role');
            });

            //create a new role form the page loaded
            $('#create_role, #edit_role').live('submit',function(event){
                event.preventDefault();
                $("#error_status").html('');
                
                if ($(this).attr('id') == 'create_role') {
                     if (roleNameValidate() == false) {
                        var okBtn  = generateCloseBtn('Ok', $confirmation);

                        $confirmation.dialog("option", "buttons", okBtn);
                        $confirmation.dialog("open");
                        return
                     }
                }
                

                if (roleIncludeExcludeValidate() == false)
                {
                    var cancelBtn  = generateCloseBtn('Cancel', $confirmation);
                    var confirmBtn = generateSubmitBtn('Confirm', $(this), $confirmation);

                    $confirmation.dialog("option", "buttons", cancelBtn.concat(confirmBtn) );
                    $confirmation.dialog("open");
                }
                else
                {   
                    $(this).ajaxSubmit(options);
                }   
            });
            
            function roleNameValidate() {
                if ($.trim($('#name').val()).length === 0) {
                    $('#confirmation span').text('You Must add Name to the role.');
                    return false;
                }
            }
            
            function roleIncludeExcludeValidate()
            {
                if ($('input[name="crxi[]"]:checked').length == 0 && $('input[name="crxx[]"]:checked').length == 0)
                {
                    $('#confirmation span').text('This role will allow complete reporting permissions for all hosts, is this what you wanted? If not, please fill in a host class regular expression.');
                    return false;
                }

                if ($('input[name="brxi[]"]:checked').length == 0 && $('input[name="brxx[]"]:checked').length === 0) 
                {
                    $('#confirmation span').text('This role will allow complete viewing permissions for all promises, is this what you wanted? If not, please fill in a bundle class regular expression.');
                    return false;
                }
                return true;
            }
            
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

// bind correct mouse behavior
var selectedClass = 'selected_item';
var clickDelay = 600;     // click time (milliseconds)
var lastClick=0;
var diffClick=0; // timestamps
var changeOnClick = false;


$( ".itemlist li" ).live('mousedown mouseup', function(e) {
    if (e.type=="mousedown") {
        lastClick = e.timeStamp; // get mousedown time */
        //$(this).toggleClass(selectedClass);
        if (!$(this).hasClass(selectedClass)) {
            $(this).addClass(selectedClass);
            changeOnClick  = false;
        }
        else
            changeOnClick = true;

    } else {
        diffClick = e.timeStamp - lastClick;

        if ( diffClick < clickDelay ) {
            // add selected class to group draggable objects
            if (changeOnClick == true) {
                $(this).toggleClass(selectedClass);
            }
           changeOnClick  = false;
        }
    }
});        
        
function addCheckbox(el, destination_id)
{
    
    if($(el).find('input').length == 0)
    { // add checkbox
        $(el).prepend('<input type="checkbox" id="" value="'+ $(el).find('span').text() + '" checked="true">');
    }
    // addname and check element
    $(el).find('input').attr({'checked':true, 'disabled':false, "name": destination_id + "[]" });
}
        
function removeCheckbox(el)
{
    var checkbox = $(el).find('input');
    if(checkbox.length != 0)
    { 
        checkbox.remove();
    }
}
        
// move element(s) from source to destination        
function bindSortable(bind_css_class_name, destinations_array)
{
    var bind_class = '.' + bind_css_class_name;
    
    if (bind_css_class_name == '' || $(bind_class).length == 0 ) {
        return;
    }
    
    var source_elem = '';
    var source_id   = '';    

    var destination_elem = '';
    var destination_id = ''; 

          $(bind_class).sortable({
            connectWith: bind_class,
            placeholder: "ui-state-highlight",
            dropOnEmpty: true,
            cursor: 'pointer',
            cancel: 'empty',
            helper: function(){ 
                var selected = $('.selected_item');
                if (selected.length === 0) { 
                    selected = $(this); 
                } 
                
                var container = $('<div/>').attr('id', 'draggingContainer'); 
                container.append(selected.clone()); 
                container.appendTo('body').show().addClass('dragged');
                
                return container;  
            },
            
            start: function(event, ui) { 
                source_elem = ui.item.parent(); // save parent id before drag
                source_id   = ui.item.parent().attr('id');
            },  
            stop: function(e, ui) {
                var $group = $('.selected_item');//.not(ui.item);
                destination_elem = $(ui.item).parent();
                destination_id = destination_elem.attr('id');

               if ($.inArray( destination_id , destinations_array) != -1)
                {
                    //add checkboxes when move items to assigned box
                    $group.each(function() {
                        addCheckbox(this, destination_id);
                    });
                }
                else { // remove checkboxes when move items to all items box
                    $group.each(function() {
                        removeCheckbox(this);
                    });
                }
                $group.clone().insertAfter($(ui.item));
 
                $group.remove();

                $(destination_elem).find('li.selected_item').removeClass('selected_item');
                
                checkEmptyList(source_elem, destination_elem);
                
               // checkAssignedCount(dest, destinations_array);
               
            },
            activate: function(event, ui) { 
            }
	}).disableSelection();
}        

$(".move_btn").live('click',function(event) {

    var source = $(this).attr('source');
    var dest   = $(this).attr('dest');
    
    var source_elem      = $('#' + source);
    var destination_elem = $('#' + dest);

    var destinations_array = new Array("crxi","crxx","brxi", 'brxx', 'roles');

    var $group = $('#' + source + ' .selected_item');


    if ($.inArray( dest, destinations_array) != -1) {
        $group.each(function() {
            addCheckbox(this, dest);
        });
    }
    else { 
        $group.each(function() {
            removeCheckbox(this)
        });
    }

    $(destination_elem).prepend($group.clone());
    $(destination_elem).find('.selected_item').removeClass('selected_item');

    $group.remove();
    
    checkEmptyList(source_elem, destination_elem);
    
    //checkAssignedCount(dest, destinations_array);
});

function checkEmptyList(source_elem, destination_elem) {
    // add empty element with message
    if ($(source_elem).children().length == 0)
    {      
        //if ($('#' + source_id).next('div.empty_list_warning').length == 0)
        if ($(source_elem).next('div.empty_list_warning').length == 0)                    
        {
            var el = $('<div class="empty_list_warning">No items assigned</div>');
            $(source_elem).after(el);
        }

        $(source_elem).next("div.empty_list_warning").show();
        $(source_elem).addClass('empty_list');
    }
    else
    { 
        $(destination_elem).next("div.empty_list_warning").remove();
        $(destination_elem).removeClass('empty_list');
    }
}

function checkAssignedCount(dest, destinations_array) {
    //TODO  
    return;
    // check count items
    if ($.inArray( dest, destinations_array) != -1) {
         if ($('#' + dest).children().length > 3) {
            $('#confirmation span').text('Too much');
         }
    }
}
    
$("#addClassRegexp" ).live('click', function(e){
    if($('#classRegexpText').val() != '') {
            $('#classList').prepend('<li class=""><span>' + $('#classRegexpText').val() + '</span></li>' );  
            $('#classRegexpText').css('border', 'inherit');
        }
        else {
            $('#classRegexpText').css('border', '1px solid red');
        }
});
</script>