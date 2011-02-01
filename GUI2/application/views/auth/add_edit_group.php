<div class="panelhead"><?php echo $operation ;?> Group</div>
	
	<div id="infoMessage"><?php echo $message;?></div>
<div class="form">
    <?php echo form_open("auth/manage_group/".$this->uri->segment(3).'/'.$this->uri->segment(4),array('id'=>$operation.'_group'));?>
    
      <p>
      <?php
       echo form_label('Group Name',$name['name']);
       echo form_input($name);
       ?>
      </p>
      
      <p>
      <?php  echo form_label('Description',$description['name'],array('class'=>'aligntop'));
       echo form_textarea($description);?>
      </p>
      
      <p id="btnholder">
      <?php echo form_submit(array('name'=>'submit','value'=>$operation,'class'=>'btn'));?>
      </p>

    <?php echo form_close();?>

</div>