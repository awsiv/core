
<div id="infoMessage"><?php echo $message;?></div>
<div class="form">
<?php echo form_open("notes/add/".$this->uri->segment(3),array('id'=>'add_cmt'));?>
      <p> 
          <?php
          echo form_textarea($note);
          ?>
      </p>
      <p>
         <input type="hidden" name="hostkey" id="hostkey" value="<?php echo $hostkey?>"/>
      </p>
                        
      <p id="btnholder">
      <?php echo form_submit(array('name'=>'submit','value'=>'comment','class'=>'btn'));?>
      </p>
</div>


