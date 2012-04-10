<div class="outerdiv grid_12">
          		
    <div class="innerdiv">

<div id="admin_content">

        <table cellspasing="5">
            <tr>
                <td>User name:</td>
                <td><?php echo $user->username ?></td>
            </tr>
           <?php if ($this->session->userdata('mode') == 'database') { ?>
            <tr>
                <td>Email:</td>
                <td><?php echo $user->email ?></td>
            </tr>
            <tr>
                <td></td>
                <td><?php  echo anchor("auth/change_password/" . $user->_id->__toString(), ' ', array('class' => 'changepassword', 'title' => 'change password')); ?></td>
            </tr>
            <?php } ?>
            <tr>
                <td colspan="2">Assigned Roles:</td>
            </tr>
            <tr>
                <td colspan="2"><div class="DragNDrop assigned dialog_box_style ui-dialog"  style="position: relative">
                        <div class="itemwrapper assigneditems">

                            <?php if (!empty($user->roles)) { 
                                $i = 1;
                                $first = true;
                                $fist_item_class = ' class="first"';


                                echo '<ul id="roles" class="roleslist itemlist assignedList">';

                                foreach ($user->roles as $item) {
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

    </div>
</div>   
    </div>
<script type="text/javascript">
/*
$(document).ready(function() {    
    var options = {
    target:  '#admin_content'  // target element(s) to be updated with server response
};    
    $('.changepassword').live('submit',function(event){
        event.preventDefault();
        $(this).ajaxSubmit(options);
    });
});*/
</script>    