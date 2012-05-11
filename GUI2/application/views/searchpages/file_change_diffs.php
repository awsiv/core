<div id="searchform" class="panel">
    
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p><label>File name: (.*+[])</label>
                <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                <span class="clear"></span>
                <input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
              
                <p><label>Match content: (.*+[])</label> 
                  <input class="searchfield" type="text" name="diff" size="80" value="<?php echo (is_array($paramArray) && isset($paramArray['diff'])) ? trim($paramArray['diff']) : ''; ?>"></p>
                <p>
                    <span class="green_btn floatright" style="margin-top: 2px"><input class="green_btn" type="submit" value="Generate Report"></input></span>
                    <span class="clear"></span>
                </p>
          
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>        
        </form>
    </div>
</div>