<div class="outerdiv">
    <div class="innerdiv">
        <p class="error">
            Error checking out the repository at <?php echo $url ?>.
            Please make sure that you have given the correct URL and credentials.
            Also make sure you have correct write permissions on policies directory under webroot. 
        </p>
        <?php if ($env == 'development') { ?> 
        <p>
            <?php foreach($errorArray as $err) { ?>
                <p class="info"><?php echo $err; ?></p>
             <?php } ?>
        </p>
        <?php } ?>
        <p class="info">Use the link below to set things right</p>
        <p class="btnholder">
             <a href="<?php echo site_url() ?>/repository/manageRepository/" class="btn">Manage your repositories.</a>
        </p>        
    </div>
</div>