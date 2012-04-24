<div id="searchform" class="panel">
    
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p><label>File name: (.*+[])</label><input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
              
                <p><label>Match content: (.*+[])</label><input class="searchfield" type="text" name="diff" size="80" value="<?php echo (is_array($paramArray) && isset($paramArray['diff'])) ? trim($paramArray['diff']) : ''; ?>"></p>
                <p>
                    <label>Show long term data: </label>
                    <input type="checkbox" name="long_term" value="true" <?php echo (is_array($paramArray) && isset($paramArray['long_term']) && $paramArray['long_term'] == 'true') ? 'checked' : ''; ?> />
                    <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                    <span class="clear"></span>
                </p>
          
            <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true));?>  
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>        
        </form>
    </div>
</div>