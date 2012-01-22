
<div id="infoMessage"><?php echo $message;?></div>
<?php
if (!empty($roles))
{ ?>
    
  <table cellpadding=0 cellspacing=10>
        <tr>
                <th>Name</th>
                <th>Description</th>
       <?php if($this->ion_auth->mode=="database"){ ?> <th>Action</th><?php }?>
        </tr>
        <?php foreach ((array)$roles as $role){ 
        ?>
        <tr>
          <td><?php echo isset($role['name'])?$role['name']:$role['displayname']?></td>
          <td><?php if(array_key_exists('description', $role))  echo $role['description']?></td>
         <?php if($this->ion_auth->mode=="database"){ ?>
          <td class="actioncol">
             <?php
             if($is_admin && $role['name'] != 'admin')
             {
              echo anchor("auth/manage_role/edit/".$role['name'], ' ',array('class'=>'edit','title'=>'edit role'));
              echo anchor("auth/delete_role/".$role['name'], ' ',array('class'=>'delete','title'=>'delete role'));
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
  <span class="btn"><?php echo anchor("auth/manage_role/create", 'Add Role',array('id'=>'add_role'))?></span>
</p>
<?php } }?>

<?php } else { ?>

<div id="infoMessage"><?php echo $this->lang->line('nothing_found'); ?></div>
<?php } ?>
