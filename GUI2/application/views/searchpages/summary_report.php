<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
           <p>Version: (.*+[])
           <span class="floatright"><label> Return host names only:</label> <input type="checkbox" name="hosts_only" value="true" <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
           <span class="clear"></span> 
           <input class="searchfield" type="text" name="version" value="<?php echo (is_array($paramArray) && isset($paramArray['version'])) ? trim($paramArray['version']) : ''; ?>"></p>
        
            <?php $this->load->view('searchpages/buttons_for_filter'); ?>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>
        </form>
    </div>
</div>