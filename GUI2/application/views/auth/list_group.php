<div class="main_info">
<span><?php echo $message;?></span>
<span><?php echo anchor("auth/manage_group/create", 'Add Group')?></span><span><?php echo anchor("auth", 'Manage users')?></span>
  <table cellpadding=0 cellspacing=10>
		<tr>
			<th>Name</th>
			<th>Description</th>
            <th colspan="2">Action</th>
		</tr>
        <?php foreach ($groups as $group){?>				
        <tr>
          <td><?php echo $group['name']?></td>
          <td><?php echo $group['description']?></td>
          <td><?php echo anchor("auth/manage_group/edit/".$group['id'], 'Edit')?></td>
          <td><?php echo anchor("auth/manage_group/delete/".$group['id'], 'delete')?></td>
        </tr>
		<?php }?>

</div>