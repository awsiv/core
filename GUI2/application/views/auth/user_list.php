
   <div id="infoMessage"><?php echo $message;?></div>
	<table cellpadding=0 cellspacing=10>
		<tr>
			<th>User Name</th>
			<th>Email</th>
			<th>Groups</th>
			<th>Status</th>
            <th>Action</th>
		</tr>
		<?php foreach ($users as $user):?>
			<tr>
				<td><?php echo $user['username']?></td>
				<td><?php echo $user['email'];?></td>
                                <td><?php if(is_array($user['group'])){
                                    $no_of_elements=count($user['group']);
                                      foreach ($user['group'] as $group) {

                                        $no_of_elements=$no_of_elements-1;
                                        if($no_of_elements==0)
                                            echo $group;
                                        else
                                             echo $group.",";
                                    }
                                }
                                else
                                {
                                    echo $user['group'];
                                 }
                                    ;?></td>

				<td><?php if($is_admin) {
                                    echo ($user['active']) ? anchor("auth/deactivate/".$user['_id'], 'Active', array('class'=>'activate')) : anchor("auth/activate/". $user['_id'], 'Inactive',array('class'=>'inactivate'));
                                }else{
                                 echo ($user['active']) ? 'Active' :  'Inactive';
                                }
                                    ?></td>

                <td class="actioncol"> 
                <?php
                if($is_admin)
                {
                echo anchor("auth/change_password/".$user['_id'], ' ',array('class'=>'changepassword','title'=>'change password'));
                echo anchor("auth/edit_user/".$user['_id'], ' ',array('class'=>'edit','title'=>'edit user'));
                echo anchor("auth/delete_user/".$user['_id'], ' ', array('class' => 'delete','title'=>'delete user'));
                }
                elseif( $user['username']==$username)
                {
                echo anchor("auth/change_password/".$user['_id'], ' ',array('class'=>'changepassword','title'=>'change password'));
                echo anchor("auth/edit_user/".$user['_id'], ' ',array('class'=>'edit','title'=>'edit my details'));
                }
             ?>
                </td>
			</tr>
		<?php endforeach;?>
	</table>
<?php if($is_admin){?>
	<p id="btnholder">
           <span class="btn"> <a id="add_user" href="<?php echo site_url('auth/create_user');?>">Add user +</a></span>
        </p>
<?php }?>