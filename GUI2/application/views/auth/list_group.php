
<div id="infoMessage"><?php echo $message;?></div>
  <table cellpadding=0 cellspacing=10>
		<tr>
			<th>Name</th>
			<th></th>
               <?php if($this->ion_auth->mode=="database"){ ?> <th>Action</th><?php }?>
		</tr>
        <?php foreach ((array)$groups as $group){?>
        <tr>
          <td><?php echo isset($group['name'])?$group['name']:$group['displayname']?></td>
          <td><?php if(array_key_exists('description', $group))  echo $group['description']?></td>
         <?php if($this->ion_auth->mode=="database"){ ?>
          <td class="actioncol">
             <?php
             if($is_admin)
             {
             echo anchor("auth/manage_group/edit/".$group['_id']->__toString(), ' ',array('class'=>'edit','title'=>'edit group'));
              echo anchor("auth/delete_group/".$group['_id']->__toString(), ' ',array('class'=>'delete','title'=>'delete group'));
             }
             ?>
          </td>
          <?php } ?>
        </tr>
		<?php }?>
  </table>
<?php 
if($this->ion_auth->mode=="database"){
if($is_admin){?>
<p id="btnholder">
  <span class="btn"><?php echo anchor("auth/manage_group/create", 'Add Group',array('id'=>'add_group'))?></span>
</p>
<?php } }?>