<div class="outerdiv grid_12">

    <div class="innerdiv">


        <div class="holder">
            <div id="admin_content" class="tables">
            <h2><?php echo $message ?></h2>

        <p>You logged in as local administrator. Current authentication mode for the Mission Portal is:<b> <?php echo  $current_auth_mode ?></b>.</p>

         <p>Local administrator don't have permission to work with users and roles if external authentication is used.</p>
         <p>Please <a href="<?php echo site_url("login/logout") ?>">log in</a> with your external credentials
             or <a href="<?php echo site_url("settings/manage") ?>">change settings</a> for the Mission portal.</p>


            </div>
        </div>
     </div>
</div>