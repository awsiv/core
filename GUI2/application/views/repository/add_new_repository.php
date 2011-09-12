
<div class="stylized">
    <form action="<?php echo isset($addFormPath) ? $addFormPath : '' ?>" method="POST" id="repo-add-form">



        <fieldset>
            <?php if (isset($errors) && is_array($errors) && !empty($errors)) { ?>

                <div class="error">
                    <ul>
                        <?php foreach ($errors as $error) { ?>
                            <li> <?php echo $error ?> </li>
                        <?php } ?>
                    </ul>
                </div>

            <?php } ?>
            <?php if ($this->session->flashdata('success')) { ?>

                <div class="success"><?php echo $this->session->flashdata('success'); ?></div> 

            <?php } ?>

            <legend>Add a new repository</legend>
            <label id="repoPath" for="repoPath">Path: </label>
            <input type="text" name="repoPath" value="<?php echo set_value('repoPath'); ?>" size="50" />

            <label for="username"> Username: </label>
            <input autocomplete="off" id="username" type="text" name="username" value="<?php echo set_value('username'); ?>" size="50" />

            <label for="password">Password: </label>
            <input autocomplete="off" id="vis-password" type="password" name="vis-password" value="<?php echo set_value('vis-password'); ?>" size="50" />
            <input autocomplete="off" id="password" type="hidden" name="password" value="<?php echo set_value('password'); ?>" size="50" />

            <input autocomplete="off" id="addnew" type="hidden" name="addnew" value="1"  />
            <label for="submit"></label>
            <input type="button" value="Add" name="test" id="submit-form" />
            <div id="ajax-loader" style="display: none;"><img src="<?php echo get_imagedir(); ?>ajax-loader.gif" /></div>
        </fieldset>
        <br />
    </form>
</div>
<script type="text/javascript">
    
   
    
    $('#submit-form').click(function(){
        
        $('#submit-form').hide();
        $('#ajax-loader').show();
        if ($("#vis-password").val()!='') {
            $.jCryption.getKeys("<?php echo site_url() ?>/repository/get_keys",function(receivedKeys) {
                keys = receivedKeys;           
                $.jCryption.encrypt($("#vis-password").val(),keys,function(encrypted) {
                    $("#password").val(encrypted);
                    var replaceVal = Array($('#vis-password').val().length + 1 ).join(" ");
                    $('#vis-password').val(replaceVal); // replace with blank values so that real value is destroyed          
                    $('#submit').removeAttr('disabled');             
                    $('#repo-add-form').submit();
                    $('#ajax-loader').hide();
                    $('#submit-form').show();
                });
            });} else {
            $('#repo-add-form').submit();
            $('#submit-form').show();
        }    
    });
    
    
</script>