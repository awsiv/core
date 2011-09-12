<p>
        <label for="username">User name</label>
        <input id="username" type="text" name="username" maxlength="50" value=""  />
</p>

<p>
        <label for="password">Password</label>
        <input id="password" type="password" name="password" maxlength="50" value=""  />
</p>
<p  id="btnholder">
        <label></label><?php echo form_submit( array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?>
</p>
