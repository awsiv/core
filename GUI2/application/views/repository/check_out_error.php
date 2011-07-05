<div class="outerdiv">
    <div class="innerdiv error">
        <p>
            There was an error encountered while checking out the repository at <?php echo $url ?>.
            Please make sure that you have given the correct URL and credentials.
        </p>
        <?php if ($env == 'development') { ?> 
        <p>
            <?php var_dump($errorArray) ?>
        </p>
        <?php } ?>
        
        <p>
            Manage you repositories. <a href="<?php echo site_url() ?>/repository/manageRepository/">Go</a>
        </p>
        
    </div>
</div>