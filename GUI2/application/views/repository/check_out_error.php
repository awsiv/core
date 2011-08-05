<div class="outerdiv">
    <div class="innerdiv">
        <p class="error">
            There was an error encountered while checking out the repository at <?php echo $url ?>.
            Please make sure that you have given the correct URL and credentials.
        </p>
        <?php if ($env == 'development') { ?> 
        <p>
            <?php var_dump($errorArray) ?>
        </p>
        <?php } ?>
        <p class="info">Use the link below to set things right</p>
        <p class="btnholder">
             <a href="<?php echo site_url() ?>/repository/manageRepository/" class="btn">Manage your repositories.</a>
        </p>
        
    </div>
</div>