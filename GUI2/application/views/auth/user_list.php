	<table cellpadding=0 cellspacing=10>
		<tr>
			<th>Users</th>
			<th>Email</th>
                        
			<th>Roles</th>
                        <?php if($this->ion_auth->mode =="database"){?>
			<th>Status</th>
                        <?php } ?>
                        <th>Action</th>
                     
		</tr>
		<?php
                $loggedinusername=isset($username)?$username:$this->session->userdata('username');
                foreach ((array)$users as $user){ 
                    // can use $user['displayname']
                        $username = isset($user['username']) ? $user['username'] : $user['name'];
                     ?>
			<tr>
				<td><?php echo $username ?></td>
				<td><?php echo isset($user['email'])?$user['email'] : "";?></td>
                                <?php 
                                $roles_str = '';
                                if(isset($user['roles'])&&is_array($user['roles']))
                                {
                                    $roles_str = implode(', ', $user['roles']);
                                } 
                                ?>                     
                                <td class="showqtip" title="<?php echo $roles_str ?>"><?php echo getCuttedText($roles_str, 5, 60) ?></td>
                          
                                <?php  if($this->ion_auth->mode=="database"){?>
				<td><?php
                                    if($is_admin) {
                                        echo ($user['active']) ? anchor("auth/deactivate/".$user['_id']->__toString(), 'Active', array('class'=>'activate')) : anchor("auth/activate/". $user['_id'], 'Inactive',array('class'=>'inactivate'));
                                    }else{
                                     echo ($user['active']) ? 'Active' :  'Inactive';
                                    }
                                
                                    ?></td>
                            <?php } ?>

                <td class="actioncol"> 
                <?php
         
 if ($is_admin || ($this->ion_auth->mode != "database" && !$this->ion_auth->is_ldap_user_exists())) {
        if ($this->ion_auth->mode != "database") {
            echo anchor("auth/edit_user_ldap/" . $username, ' ', array('class' => 'edit', 'form' => 'edit_user', 'title' => 'edit user'));
        } else {
            echo anchor("auth/edit_user/" . $user['_id']->__toString(), ' ', array('class' => 'edit', 'form' => 'edit_user', 'title' => 'edit user'));
        }

        if ($this->ion_auth->mode == "database") {
            if ($user['username'] != $loggedinusername) {
                if ($username != $fall_back_for) { // if this is not "admin" user
                    echo anchor("auth/delete_user/" . $user['_id']->__toString(), ' ', array('class' => 'delete', 'title' => 'delete user'));
                }
            } elseif ($user['username'] == $loggedinusername) {
                echo anchor("auth/change_password/" . $user['_id']->__toString(), ' ', array('class' => 'changepassword', 'title' => 'change password'));
            }
        }
    }
    ?>
                </td>
               
			</tr>
		<?php }?>
	</table>
<?php
if($this->ion_auth->mode =="database"){
   if($is_admin){?>
	<p id="btnholder">
           <a href="<?php echo site_url('auth/create_user');?>" id="add_user" class="green_btn"><span>Add user +</span></a>
        </p>
<?php }
}?>