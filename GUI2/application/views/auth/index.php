<div class='mainInfo'>
<p><?php  echo "Logged in: $username" ; ?>&nbsp;<a href="<?php echo site_url('auth/logout'); ?>">Logout</a></p>
	<div id="infoMessage"><?php echo $message;?></div>
	<table cellpadding=0 cellspacing=10>
		<tr>
			<th>First Name</th>
			<th>Last Name</th>
			<th>Email</th>
			<th>Group</th>
			<th>Status</th>
            <td colspan="2">Action</td>
		</tr>
		<?php foreach ($users as $user):?>
			<tr>
				<td><?php echo $user['first_name']?></td>
				<td><?php echo $user['last_name']?></td>
				<td><?php echo $user['email'];?></td>
				<td><?php echo $user['group_description'];?></td>
				<td><?php echo ($user['active']) ? anchor("auth/deactivate/".$user['id'], 'Active') : anchor("auth/activate/". $user['id'], 'Inactive');?></td>
                <td><?php echo anchor("auth/edit_user/".$user['id'], 'Edit')?></td>
                <td><?php echo anchor("auth/delete/".$user['id'], 'delete')?></td>
			</tr>
		<?php endforeach;?>
	</table>
	
	<p><a href="<?php echo site_url('auth/create_user');?>">Add user</a></p>
	<p><a href="<?php echo site_url('auth/manage_group');?>">Manage Groups</a></p>
	
	
</div>
