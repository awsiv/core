<div id="formInfoMessage"><?php echo $message; ?></div>
<table cellpadding=0 cellspacing=10>
		<tr>
			<th>Users</th>
			<th>Email</th>
                        
			<th>Roles</th>
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
                                <td class="<?php if(!empty($roles_str)) { ?>showqtip <?php } ?>" title="<?php echo $roles_str ?>"><?php echo getCuttedText($roles_str, 5, 60) ?></td>
                          
                           

                <td class="actioncol"> 
                <?php
         
 if ($is_admin || ($this->ion_auth->mode != "internal" && !$this->ion_auth->is_ldap_user_exists())) {
        if ($this->ion_auth->mode != "internal") {
            echo anchor("auth/edit_user_ldap/" . $username, ' ', array('class' => 'edit', 'form' => 'edit_user', 'title' => 'edit user'));
        } else {
            echo anchor("auth/edit_user/" . $username, ' ', array('class' => 'edit', 'form' => 'edit_user', 'title' => 'edit user'));
        }

        if ($this->ion_auth->mode == "internal") {
            if ($user['username'] != $loggedinusername) {
                ///if ($username != $fall_back_for) { // if this is not "admin" user
                    echo anchor("auth/delete_user/" . $username, ' ', array('class' => 'delete', 'title' => 'delete user'));
               // }
            } elseif ($user['username'] == $loggedinusername) {
                echo anchor("auth/change_password/" . $username, ' ', array('class' => 'changepassword', 'title' => 'change password'));
            }
        }
    }
    ?>
                </td>
               
			</tr>
		<?php }?>
	</table>
<?php
if($this->ion_auth->mode =="internal"){
   if($is_admin){?>
	<p id="btnholder">
           <a href="<?php echo site_url('auth/create_user');?>" id="add_user" class="green_btn"><span>Add user +</span></a>
        </p>
<?php }
}?>