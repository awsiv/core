<div class="outerdiv grid_12">

    <div class="innerdiv">
        <div id="infoMessage"><?php echo $message; ?></div> 
        <div id="darktabs" class="user_roles_management">
            <div class="ui-tabs ui-widget ui-widget-content ui-corner-all" style="margin-left: 0px;">        
                <ul class="admin_menu ui-tabs-nav ui-helper-reset ui-helper-clearfix ui-widget-header ui-corner-all">
                    <li class="ui-state-default ui-corner-top ui-tabs-selected ui-state-active ">
                        <a href="<?php echo site_url('auth/index'); ?>">Users</a>
                    </li>
                    <li class="ui-state-default ui-corner-top">
                        <a href="<?php echo site_url('auth/manage_role'); ?>">Roles</a>
                    </li>
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
        },
        error: 
            function() {
            $('.blockUI').hide();   
            $(document).unblock();
            $.unblockUI();
        }
            
    });     
        
    $(document).ajaxStop(function(){
        setTimeout(function() {
            $("#infoMessage, #formInfoMessage").fadeOut(500)
        }, 10000);
    });
        
    // global variables for ajax and drag&drop
    var options = {
        target:  '#admin_content'  // target element(s) to be updated with server response
    };
        
    var $group = '';  //items with .selected_item class       
    var li_pos = '';  // LI position to insert on drag
    var ul_pos = '';  // UL position to insert on drag

    var maxAllowedClasses = 5;
    var classesArray = ['crxi','crxx'];
   
    // options for edit forms
    var edit_form_options =  {
        target:  '#edit_form_wrapper',
        dataType: 'json',
        beforeSubmit: function(arr, $form, options)
        {
        },
        success: function (responseObj, statusText, xhr, $form) {
        
            if (responseObj.status == 'all_ok')
            {
                $('#admin_content').html(responseObj.responseText);
                $('#edit_form_wrapper').remove();
            }
            else
            {
                $("#formInfoMessage").html(responseObj.responseText);
                $("#formInfoMessage").show();
                window.scrollTo(null, ($('#formInfoMessage').position().top - 20));
            }
            return;
        }   
    };
        
    $(document).ready(function() {


        // check if usr manually check/uncheck items
        $("ul#crxi input, ul#crxx input").live("click", function(){ 
            if ($(this).is(':checked')) {
                var parentId = $(this).parent().parent().attr('id');

                if($('#' + parentId + ' input[type="checkbox"]:checked').length > maxAllowedClasses) {
                    var el = $(this);

                    $('#confirmation span').text('Please use less than ' + maxAllowedClasses + ' classes. Using more classes could affect system productivity');
                    var cancelBtn   = generateDialogBtn('Cancel',   $confirmation, function(){el.attr('checked', false); return false} );
                    var continueBtn = generateDialogBtn('Continue', $confirmation, function(){el.attr('checked', true); return false});    
                    $confirmation.dialog("option", "buttons", [continueBtn,cancelBtn]);
                    $confirmation.dialog("open"); 
                }
            }
        });  
        /************************************************************************************/  
        //              Buttons
        /************************************************************************************/ 


        //loading the create user page from server to add the user
        $('#add_user').live('click',function(event) {
            event.preventDefault();
            $("#error_status").html('');       
            attach_edit_form(this, $(this).attr('form'));
        });
        
        //submitting the create user form
        $('#create_user, #edit_user').live('submit',function(event) {
            event.preventDefault();
            $("#error_status").html('');   
            $(this).ajaxSubmit(edit_form_options);
        });

        //loading the change password in admin_content
        $('a.changepassword').live('click',function(event){
            event.preventDefault();
            $("#error_status").html('');
            var path=$(this).attr('href');
            $("#admin_content").load(path);
        });

        //submitting the form ajaxically to the page in form action and loading the result in admin_content
        $('#change_password').live('submit',function(event){
            event.preventDefault();
            $(this).ajaxSubmit(options);
        });


        //loading the  role create page in the  admin area ajaxcially
        $('#add_role').live('click',function(event){
            event.preventDefault();
            attach_edit_form(this, 'role');
        });

        //load the result from the server after delete page is called
        $('a.delete').live('click',function(event){
            event.preventDefault();
            $("#error_status").html('');
            var msg='Are you sure you want to delete this role?';
            
            if($(event.target).attr('title') == 'delete user'){
               msg= 'Are you sure you want to delete this user?';
            }
            $('#confirmation span').text(msg);

            // create buttons - cancel and confirm
            var cancelBtn  = generateDialogBtn('Cancel', $confirmation);
        
            var args=[];
            args.domElementId = options.target;
            args.link = $(this).attr('href');
        
            var confirmBtn = generateDialogBtn('Confirm', $confirmation, loadContent, args);

            $confirmation.dialog("option", "buttons", [cancelBtn,confirmBtn] );
            $confirmation.dialog("open");
        });

        //loading the edit page in admin_content of the admin area
        $('a.edit').live('click',function(event){
            event.preventDefault();
            attach_edit_form(this, $(this).attr('form'));
        });

        $(".arrows a").live('click',function(event) {
            var source_id = $(this).attr('source');
            var destination_id   = $(this).attr('dest');
        
            var source_elem      = $('#' + source_id);
            var destination_elem = $('#' + destination_id);
        
            var destinations_array = new Array('roles');
        
            //check if we are trying to move into same block
            if ($('#'+ source_id + ' .selected_item').length == 0)
            {
                return false;
            }
        
            li_pos = ul_pos = '';
            var args = [];
            args.destination_id = destination_id;
            args.destination_elem = destination_elem;
            args.destinations_array = destinations_array;
            args.source_id = source_id;
            args.source_elem = source_elem;
            args.e = event;
        

            checkAndMove(args);

        
            checkEmptyList(source_elem, destination_elem);
       
        });
    
        //loading all the pages in admin content after clicking the items in admin index menu
        $('.admin_menu li a').click(function(event){
            event.preventDefault();
            var path=$(this).attr('href');
            $("#error_status").html('');
            $('#edit_form_wrapper').html('');
            $("#admin_content").load(path);
            $(this).parent().addClass('ui-tabs-selected ui-state-active ').siblings().removeClass('ui-tabs-selected ui-state-active');
        });



        //create a new role form the page loaded
        $('#create_role, #edit_role').live('submit',function(event){
            event.preventDefault();
            $("#error_status").html('');

            if (roleNameValidate($('#name').val()) == false || roleDescriptionValidate($('#description').val())==false) {
                var okBtn = generateDialogBtn('Ok', $confirmation);
                $confirmation.dialog("option", "buttons", [okBtn]);
                $confirmation.dialog("open");
                return
            }
            
            if (includeClassValidate() == false) {
                var okBtn = generateDialogBtn('Ok', $confirmation);
                $confirmation.dialog("option", "buttons", [okBtn]);
                $confirmation.dialog("open");
                return
            }
           
            if (roleIncludeExcludeValidate() == false)
            {
                var $form = $(this);
                var cancelBtn  = generateDialogBtn('Cancel', $confirmation);
                var confirmBtn = generateDialogBtn('Confirm', $confirmation, function(){$form.ajaxSubmit(edit_form_options);});

                $confirmation.dialog("option", "buttons", [cancelBtn,confirmBtn] );
                $confirmation.dialog("open");
            }
            else
            {
                $(this).ajaxSubmit(edit_form_options);
            }  
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

        $("#addClassRegexp" ).live('click', function(e){
            if($('#classRegexpText').val() != '') {
                $('#classList').prepend('<li class=""><span>' + $('#classRegexpText').val() + '</span></li>' );  
                $('#classRegexpText').css('border', 'inherit');
            }
            else {
                $('#classRegexpText').css('border', '1px solid red');
            }
        });
    
        /************************************************************************************/    


        // bind correct mouse behavior
        var selectedClass = 'selected_item';
        var clickDelay = 600;     // click time (milliseconds)
        var lastClick=0;
        var diffClick=0; // timestamps
        var changeOnClick = false;
        var lastParentClicked = '';

        $(".itemlist li" ).live('mousedown mouseup', function(e) {
            // do not aloow to select items from 2 lists
            if (lastParentClicked == '') {
                lastParentClicked = $(this).parent().attr('id');
            }
            else
            {
                if (lastParentClicked != $(this).parent().attr('id'))
                {
                    $("#" + lastParentClicked + ' li.' + selectedClass).removeClass(selectedClass);
                    lastParentClicked = $(this).parent().attr('id');
                }    
            }    
    
            if (e.type=="mousedown") {
                lastClick = e.timeStamp; // get mousedown time */

                if (!$(this).hasClass(selectedClass)) {
                    $(this).addClass(selectedClass);
                    $group = $('.selected_item');
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
                        $group = $('.selected_item');
                    }
                    changeOnClick  = false;
                }
            }
        });        
        
        /*********************** common functions  ****************************************************************/        

        
        function includeClassValidate() {
            if (getInputValues('#classes_inc_exc .includes').length == 0 )
            {
                $('#confirmation span').html('Role must have at least one <b>include</b> class.');
                return false;
            }
        }
        
        function roleIncludeExcludeValidate()
        {
           if (getInputValues('#classes_inc_exc .includes').length == 0 && getInputValues('#classes_inc_exc .excludes').length == 0)
            {
                $('#confirmation span').text('This role will allow complete reporting permissions for all hosts, is this what you wanted? If not, please fill in a host class regular expression.');
                return false;
            }

            if (getInputValues('#bundles_inc_exc .includes').length == 0 && getInputValues('#bundles_inc_exc .excludes').length == 0)
            {
                $('#confirmation span').text('This role will allow complete viewing permissions for all promises, is this what you wanted? If not, please fill in a bundle class regular expression.');
                return false;
            }
    
            return true;
        }

        function attach_edit_form(elem, form) {
            var parent = $(elem).parent().parent(); //get parent element   
            //delete element
            if($("#edit_form_wrapper").length) {
                $("#edit_form_wrapper").remove();
            }
    
            if ($(parent).get(0).tagName.toLowerCase() == 'tr')
            {
                var colspan = $(parent).children('td').length;
                //create element with the same type as parent
                $('<tr id="edit_form_wrapper"><td colspan="' + colspan +'"><div id="edit_form"></div></td></tr>').insertAfter(parent);  
            }
            else  {
                $('<div id="edit_form_wrapper"><div id="edit_form"></div></div>').insertAfter(parent);  
            }
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

        function getSelectedCheckboxes(parentId) {
            var res = [];
            $('#' + parentId + ' input[type="checkbox"]:checked').each(function() { 
                res.push($(this).val());
            });
    
            return res;
        }
        
        function getInputValues(selector) {
            var res = [];
             $(selector + ' input[type="text"]').each(function() { 
                 if($(this).val() != '')
                    res.push($(this).val());
            });
            return res;
        }
        
        function addFinders() {
            $('#classes_inc_exc').contextfinder({
                finder_title: 'Classes',
                baseUrl: '<?php echo site_url() ?>',
                embedded: true,
                
                include_field_name: 'crxi',
                exclude_field_name: 'crxx',
                useFinder: 'classfinder',                
                
                HTML_ID: 'classes_inc_exc',
                setContextClbkFnc:function() { 
                }, 
                complete:function(event,data){
                }
            }); 
           
            $('#bundles_inc_exc').contextfinder({
                finder_title: 'Bundles',
                baseUrl: '<?php echo site_url() ?>',
                embedded: true,

                include_field_name: 'brxi',
                exclude_field_name: 'brxx',
                useFinder: 'policyfinder',
                
                tooltips:{select_item: 'Select bundle'},
                
                HTML_ID: 'bundles_inc_exc',
                setContextClbkFnc:function() { 
                }, 
                complete:function(event,data){
                }
            });         
            
        }

        function roleNameValidate(name) {
            var name = $.trim(name);
    
            if (name.length == 0) {
                $('#confirmation span').text('You must add Name to the role.');
                return false;
            }
    
            var roleRegex = /^[A-Za-z0-9_]+$/; //letters, numbers and underscore
  
            if (!roleRegex.test(name)) { 
                $('#confirmation span').text('Please use only letters, numbers and underscore for Role name.');
                return false; 
            }
        }

        function roleDescriptionValidate(name) {
            var name = $.trim(name);
    
            if (name.length == 0) {
                $('#confirmation span').text('You must add Description to the role.');
                return false;
            }    
        }

    });  // end of document.ready

    /*-------------------------------------------------------------------------------------------------*/


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
            connectToSortable: '.classlist',
            placeholder: "ui-state-highlight",
            dropOnEmpty: true,
            cursor: 'pointer',
            cancel: 'empty',
            addClasses: true,
            
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
            beforeStop: function(event, ui) {
                // Find correct placeholder
                // if insert inside list - get position inside LI
                li_pos = ui.item.prev('li');
                if(li_pos.length == 0) // this mean that we are trying to insert as first element of li
                {
                    ul_pos =  ui.item.parent(); //
                }    
            },
            start: function(event, ui) { 
                source_elem = ui.item.parent(); // save parent id before drag
                source_id   = ui.item.parent().attr('id');
                
            },  
            stop: function(e, ui) {
                destination_elem = $(ui.item).parent();
                destination_id = destination_elem.attr('id');
                
                // do not move items if source and destination is the same items
                if(destination_id == source_id)
                {
                    return false;
                }    
                var args = [];
                args.destination_id     = destination_id;
                args.destination_elem   = destination_elem;
                args.destinations_array = destinations_array;
                
                args.source_id   = source_id;
                args.source_elem = source_elem;
                args.e = e;

                
                checkAndMove(args);
              
            }
        }).disableSelection();
    }

    function prepareMove(args) {
        moveItem(args.destination_id, args.destination_elem, args.destinations_array, args.source_elem, args.group);
    }

    function moveItem(destination_id, destination_elem, destinations_array, source_elem ) {
    
        // move from available to assigned
        if ($.inArray( destination_id , destinations_array) != -1)
        {
            //add checkboxes when move items to assigned box
            $group.each(function() {
                addCheckbox(this, destination_id);
            });
        }
        else {   // move from  assigned to available
            // remove checkboxes when move items to all items box
            $group.each(function() {
                removeCheckbox(this);
            });
        }

        // append elements depend on position
        if (li_pos.length != 0) {
            li_pos.after($group.clone());
        }
        else if(ul_pos.length != 0)
        {
            ul_pos.prepend($group.clone());
        }
        else
        {
            el = $group.clone();
            destination_elem.append(el);
        }

        $group.remove();

        $(destination_elem).find('li.selected_item').removeClass('selected_item');
        $(source_elem).find('li.selected_item').removeClass('selected_item');

        // check only for classes
        checkEmptyList(source_elem, destination_elem);
               
        //reset positions
        li_pos = ul_pos = '';
        $group = [];
               
    }

    function addCheckbox(el, destination_id)
    {

        if($(el).find('input').length == 0)
        { // add checkbox
            $(el).prepend('<input type="checkbox" id="" value="'+ $(el).find('span').text() + '" checked="true">');
        }
        // addname and check element
        $(el).find('input').attr({'checked':true, 'disabled':false, "name": destination_id + "[]" });
    }
        
    function removeCheckbox(el) {
        var checkbox = $(el).find('input');
        if(checkbox.length != 0)
        { 
            checkbox.remove();
        }
    }

    function checkEmptyList(source_elem, destination_elem) {
       
        // add empty element with message
        if ($(source_elem).children().length == 0)
        {      
            //if ($('#' + source_id).next('div.empty_list_warning').length == 0)
            if ($(source_elem).next('div.empty_list_warning').length == 0)                    
            {
                //if (source_elem.id)
                var el = '';
                if (source_elem.attr('id') == 'roles')
                    el = $('<div class="empty_list_warning">No items assigned</div>');
                else
                    el = $('<div class="empty_list_warning">No roles left</div>');
                
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

    function checkAssignedCount(dest, destinations_array, args) {
        // check count items
        if ($.inArray(dest, destinations_array) != -1) {
            if ($('#' + dest + ' input[type="checkbox"]:checked').length + $group.length > maxAllowedClasses) {
                $('#confirmation span').text('Please use less than ' + maxAllowedClasses + ' classes. Using more classes could affect system productivity');
                var cancelBtn   = generateDialogBtn('Cancel',   $confirmation, function(){$(args.destination_elem).find('li.selected_item').removeClass('selected_item'); return false} );
                var continueBtn = generateDialogBtn('Continue', $confirmation, prepareMove, args);    
                $confirmation.dialog("option", "buttons", [continueBtn,cancelBtn]);
                $confirmation.dialog("open"); 
                return false;
            }
        }
        return true;
    }


    function checkAndMove(args) {
        if ($.inArray(args.destination_id, classesArray) != -1 && args.destination_id != args.source_id)
        {
            if (checkAssignedCount(args.destination_id, classesArray, args) == true) {
                moveItem(args.destination_id,args.destination_elem, args.destinations_array, args.source_elem );
            }
            else
            {
                args.e.preventDefault(); 
            }
        }
        else
        {
            moveItem( args.destination_id, args.destination_elem, args.destinations_array, args.source_elem);
        }
    }

    function loadContent(args) {
        $(args.domElementId).load(args.link, 
        function(responseText, textStatus, XMLHttpRequest) {
            switch (XMLHttpRequest.status) {
                case 200: break;
                case 401:
                case 404:
                    $('#error_status').html('<p class="error">' + responseText + '</p>');
                    break;
                default:
                    $('#error_status').html('<p class="error">' + '<?php echo $this->lang->line('500_error'); ?>' + '</p>');
                    break;
                }
            }
        );
        }

        /****************************** DIALOG **********************************************/    

        //dialog object                
        var $confirmation = $('#confirmation').dialog({
            autoOpen: false,
            modal: true,
            data: '',
            resizable: false,
            beforeClose: '',
            buttons: [],
            create: function(event, ui) {
                // weird way to get clicked button
                $(".ui-dialog-buttonset button").live("click", function(event, self){ 
                    $confirmation.dialog.data = {clicked: $(this).attr('name')};
                }); 
            },
            open: function() 
            {
                $(this).parent().find('.ui-dialog-buttonpane').find('button:last').focus()
            }
        });    


        function generateDialogBtn(label, dialogObj, callbackFnc, args) {
            var btn = {
                text: label,
                value: label,
                name: label,
                click: function()
                {
                    dialogObj.dialog('close');
                    //form.ajaxSubmit(options);
                    if (callbackFnc != '' && $.isFunction(callbackFnc)) {
                        callbackFnc.call(this, args);
                    }
                }
            };
            return btn;
        }       
</script>