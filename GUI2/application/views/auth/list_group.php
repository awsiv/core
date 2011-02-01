
<div id="infoMessage"><?php echo $message;?></div>
  <table cellpadding=0 cellspacing=10>
		<tr>
			<th>Name</th>
			<th></th>
            <th colspan="2">Action</th>
		</tr>
        <?php foreach ($groups as $group){?>				
        <tr>
          <td><?php echo $group['name']?></td>
          <td><?php if(array_key_exists('description', $group))  echo $group['description']?></td>
          <td><?php echo anchor("auth/manage_group/edit/".$group['_id'], 'Edit',array('class'=>'edit'))?></td>
          <td><?php echo anchor("auth/delete_group/".$group['_id'], 'delete',array('class'=>'delete'))?></td>
        </tr>
		<?php }?>
  </table>

<p id="btnholder">
  <span class="btn"><?php echo anchor("auth/manage_group/create", 'Add Group',array('id'=>'add_group'))?></span>
</p>
