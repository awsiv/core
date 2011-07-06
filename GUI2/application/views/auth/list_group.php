
<div id="infoMessage"><?php echo $message;?></div>
  <table cellpadding=0 cellspacing=10>
		<tr>
			<th>Name</th>
			<th></th>
                <th>Action</th>
		</tr>
        <?php foreach ($groups as $group){?>				
        <tr>
          <td><?php echo $group['name']?></td>
          <td><?php if(array_key_exists('description', $group))  echo $group['description']?></td>
          <td class="actioncol">
             <?php
             if($is_admin)
             {
             echo anchor("auth/manage_group/edit/".$group['_id']->__toString(), ' ',array('class'=>'edit','title'=>'edit group'));
              echo anchor("auth/delete_group/".$group['_id']->__toString(), ' ',array('class'=>'delete','title'=>'delete group'));
             }
             ?>
          </td>
        </tr>
		<?php }?>
  </table>
<?php if($is_admin){?>
<p id="btnholder">
  <span class="btn"><?php echo anchor("auth/manage_group/create", 'Add Group',array('id'=>'add_group'))?></span>
</p>
<?php } ?>