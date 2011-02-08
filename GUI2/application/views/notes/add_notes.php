
<div class="form">
<?php echo form_open("notice/add".$this->uri->segment(3),array('id'=>'add_cmt'));?>
     <p>
        <?php
         echo form_label('User Name',$user_name['user_name']);
         echo form_input($user_name);
        ?>
     </p>
      <p> 
          <?php  echo 
          form_label('Description',$description['name'],array('class'=>'aligntop'));
          echo form_textarea($description);?>

          <textarea name="comment_text" rows="10" cols="50"> </textarea>
      </p>
      <p>
         <input type="hidden" name="hostkey" id="hostkey" value="<?php echo $hostkey?>"/>
      </p>
                        
      <p id="btnholder">
      <?php echo form_submit(array('name'=>'submit','value'=>'comment','class'=>'btn'));?>
      </p>
</div>


