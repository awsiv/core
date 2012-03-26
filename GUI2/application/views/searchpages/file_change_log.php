<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>File name: (.*+[])<input class="searchfield" type="text" name="name" title="Regular expression supported" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
            <div id="morefilters" style="display:none">
                <p>Show long term data: <input type="checkbox" name="long_term" value="true" <?php echo (is_array($paramArray) && isset($paramArray['long_term']) && $paramArray['long_term'] == 'true') ? 'checked' : ''; ?> /></p>
            </div>
            <p>
                <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
                <span class="clear"></span>
            </p>
            <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true)); ?> 
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>    
        </form>
    </div>
</div>