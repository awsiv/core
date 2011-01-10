<div class='mainInfo'>

	<h1><?php echo $operation ;?></h1>
	<p>Please enter the groups information below.</p>
	
	<div id="infoMessage"><?php echo $message;?></div>
	
    <?php echo form_open("auth/manage_group/".$this->uri->segment(3).'/'.$this->uri->segment(4));?>
    
      <p>Group Name:<br />
      <?php echo form_input($name);?>
      </p>
      
      <p>Description:<br />
      <?php echo form_textarea($description);?>
      </p>
      
      <p><?php echo form_submit('submit', $operation);?></p>

    <?php echo form_close();?>

</div>