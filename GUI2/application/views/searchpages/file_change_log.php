<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>File name: (.*+[])
                <span class="floatright"> <label>Return host names only: </label><input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                <span class="clear"></span> 
               <input class="searchfield" type="text" name="name" title="Regular expression supported" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          
            <p>
                Show long term data: <input type="checkbox" name="long_term" value="true" <?php echo (is_array($paramArray) && isset($paramArray['long_term']) && $paramArray['long_term'] == 'true') ? 'checked' : ''; ?> />
                <span class="green_btn floatright" style="margin-top: 2px"><input class="green_btn" type="submit" value="Generate Report"></input></span>
                <span class="clear"></span>
            </p>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>    
        </form>
    </div>
</div>