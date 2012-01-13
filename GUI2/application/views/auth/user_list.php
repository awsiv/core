
   <div id="infoMessage"><?php echo $message;?></div>
	<table cellpadding=0 cellspacing=10>
		<tr>
			<th>Users</th>
			<th>Email</th>
                        
			<th>roles</th>
                        <?php if($this->ion_auth->mode =="database"){?>
			<th>Status</th>
                        <?php } ?>
                        <th>Action</th>
                     
		</tr>
		<?php
                $username=isset($username)?$username:$this->session->userdata('username');
                foreach ((array)$users as $user){ 
                        $username=isset($user['username'])?$user['username']:$user['displayname'];
                     ?>
			<tr>
				<td><?php echo $username ?></td>
				<td><?php echo isset($user['email'])?$user['email']:"";?></td>
                     
                                <td><?php if(isset($user['roles'])&&is_array($user['roles'])){
                                    $no_of_elements=count($user['roles']);
                                      foreach ($user['roles'] as $role) {

                                        $no_of_elements=$no_of_elements-1;
                                        if($no_of_elements==0)
                                            echo $role;
                                        else
                                             echo $role.", ";
                                    }
                                }
                                else
                                {
                                    echo isset($user['roles'])?$user['roles']:"";
                                 }
                                    ;?></td>
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
                    if($is_admin || ($this->ion_auth->mode !="database" && !$this->ion_auth->is_ldap_user_exists()))
                    {
                        if($this->ion_auth->mode=="database"){
                        echo anchor("auth/change_password/".$user['_id']->__toString(), ' ',array('class'=>'changepassword','title'=>'change password'));
                        }
                    
                       if($this->ion_auth->mode !="database"){
                           echo anchor("auth/edit_user_ldap/".$username, ' ',array('class'=>'edit','title'=>'edit user'));
                        }else{
                           echo anchor("auth/edit_user/".$user['_id']->__toString(), ' ',array('class'=>'edit','title'=>'edit user'));  
                        }
                        
                     if($this->ion_auth->mode=="database"){
                        if( $user['username']!=$username){
                          echo anchor("auth/delete_user/".$user['_id']->__toString(), ' ', array('class' => 'delete','title'=>'delete user'));
                          }
                        elseif( $user['username']==$username)
                        {
                        echo anchor("auth/change_password/".$user['_id']->__toString(), ' ',array('class'=>'changepassword','title'=>'change password'));
                       // echo anchor("auth/edit_user/".$user['_id']->__toString(), ' ',array('class'=>'edit','title'=>'edit my details'));
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
           <span class="btn"> <a id="add_user" href="<?php echo site_url('auth/create_user');?>">Add user +</a></span>
        </p>
<?php }
}?>