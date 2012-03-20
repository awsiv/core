<div id="edit_form">
    <div id="infoMessage"><?php echo $message; ?></div>
    <div class="form">

        <?php
        if ($op == 'edit')
            echo form_open("auth/edit_user/" . $this->uri->segment(3), array('id' => 'edit_user'));
        else
            echo form_open("auth/create_user", array('id' => 'create_user'));
        ?>

        <p>
            <?php
            echo form_label('User Name', $user_name['name']);
            echo form_input($user_name);
            ?>
        </p>

        <p>
            <?php
            echo form_label('Email', $email['name']);
            echo form_input($email);
            ?>
        </p>
            <?php if ($op == 'create') { ?>
            <p>
        <?php 
            echo form_label('Password', $password['name']);
            echo form_input($password);
        ?>
            </p>

            <p>
            <?php echo form_label('Confirm Password', $password_confirm['name']);
            echo form_input($password_confirm);
            ?>
            </p>

        <?php } else { ?>
            <p>
                <label for="reset_password">Reset Password</label>
                <input type=checkbox name="reset_password"> 
            </p>
    <?php } ?>

        <table cellpadding="0" cellspasing ="0" border="1"  id="maintable">
            <tr>
                <td colspan="3">  
                    <h2>Roles</h2>
    <?php ?></td>
            </tr>     

            <tr>
                <td class="edit_cell">
                    <div class="DragNDrop assigned dialog_box_style ui-dialog"  style="position: relative">
                        <div class="ui-dialog-titlebar"><h6>Assigned</h6></div>
                        <div class="itemwrapper assigneditems" id="">

                            <?php if (!empty($user_roles)) { 
                                $i = 1;
                                $first = true;
                                $fist_item_class = ' class="first"';


                                echo '<ul id="roles" class="roleslist itemlist assignedList">';

                                foreach ($user_roles as $item) {
                                    echo '<li " ' . ($first === TRUE ? $fist_item_class : "") . ' >';
                                    $data = array(
                                        'name' => $item['name'],
                                        'id' => $item['value'],
                                        'value' => $item['value'],
                                        'checked' => TRUE,
                                        'style' => '',
                                        'class' => "selectit"
                                    );
                                    echo form_checkbox($data);
                                    echo form_label($item['value'], $item['value']);
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
                <td class="arrows">
                    <a href="javascript:void(0);" class="left" dest="roles" source="all_roles" title="&laquo; Left">&nbsp;</a>
                    <a href="javascript:void(0);" class="right" dest="all_roles" source="roles" title="Right &raquo;">&nbsp;</a>
                </td>
                <td rowspan="2">
                    <div class="DragNDrop assigned dialog_box_style ui-dialog" style="position: relative">
                        <div class="ui-dialog-titlebar"><h6>Available</h6></div>
                        <div class="itemwrapper assigneditems" id="">

                            <?php if (!empty($roles)) { 
                                $i = 1;
                                $first = true;
                                $fist_item_class = ' class="first"';


                                echo '<ul id="all_roles" class="roleslist itemlist assignedList">';

                                foreach ($roles as $item) {
                                    echo '<li " ' . ($first === true ? $fist_item_class : "") . ' >';
                                    echo '<span>' . $item['value'] . '</span>';
                                    echo "</li>";
                                    $i++;
                                    $first = false;
                                }

                                echo '</ul>';

                                } else { ?>
                                <ul id="all_roles" class="rolelist itemlist assignedList empty_list"></ul>
                                <div class="empty_list_warning">No items available </div>
                        <?php } ?>
                        </div>
                    </div>     
                </td>   
            </tr>            
        </table>     

        <p id="btnholder"><?php
            if ($op == 'edit') { ?>
               <span class="green_btn">
                    <input type="submit" value="Update User" class="green_btn" id="edit_user">
               </span>
             <?php
            } else { ?>
               <span class="green_btn">
                    <input type="submit" value="Create User" class="green_btn" id="create_user">
               </span>            
            <?php }  ?>
        </p>
            <?php echo form_close(); ?>
    </div>
</div>    