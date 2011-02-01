
        <div id="infoMessage"><?php echo $message;?></div>
	<table cellpadding=0 cellspacing=10>
		<tr>
			<th>User Name</th>
			<th>Email</th>
			<th>Groups</th>
			<th>Status</th>
            <th colspan="3">Action</th>
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

				<td><?php echo ($user['active']) ? anchor("auth/deactivate/".$user['_id'], 'Active') : anchor("auth/activate/". $user['_id'], 'Inactive');?></td>
                <td><?php echo anchor("auth/edit_user/".$user['_id'], 'Edit',array('class'=>'edit'))?></td>
                <td><?php echo anchor("auth/delete_user/".$user['_id'], 'delete', array('class' => 'delete'))?></td>
                <td><?php echo anchor("auth/change_password/".$user['_id'], 'change password')?></td>
			</tr>
		<?php endforeach;?>
	</table>

	<p id="btnholder">
           <span class="btn"> <a id="add_user" href="<?php echo site_url('auth/create_user');?>">Add user</a></span>
        </p>
