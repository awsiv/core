

    <div class="panelhead">Deactivate User</div>
    <div class="form">
    <p>Are you sure you want to deactivate the user '<?php echo $user->username; ?>'</p>

    <?php echo form_open("auth/deactivate/" . $user->_id,array('id'=>'deactivate_user')); ?>

    <p>
        <input type="radio" name="confirm" value="yes" checked="checked" />
        <label for="confirm">Yes</label>

         <input type="radio" name="confirm" value="no" />
        <label for="confirm">No</label>
      
    </p>

    <?php echo form_hidden($csrf); ?>
    <?php echo form_hidden(array('id' => $user->_id)); ?>

   <p id="btnholder"><?php echo form_submit(array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?></p>

    <?php echo form_close(); ?>
</div>

