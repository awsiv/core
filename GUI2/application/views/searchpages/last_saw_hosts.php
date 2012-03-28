 <div id="searchform" class="panel">
 <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p><label>Remote host name: (.*+[])</label><input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
           
          <div id="morefilters" style="display:none">
          <p><label>Remote IP address: (.*+[])<label><input class="searchfield" type="text" name="address" value="<?php echo (is_array($paramArray) && isset($paramArray['address'])) ? trim($paramArray['address']) : ''; ?>"></p>
          <p><label>Remote host key:</label><input class="searchfield" type="text" name="key" value="<?php echo (is_array($paramArray) && isset($paramArray['key'])) ? trim($paramArray['key']) : ''; ?>"></p>
          <p><label>Minimum hours ago:</label><input class="searchfield" type="text" name="ago" value="<?php echo (is_array($paramArray) && isset($paramArray['ago'])) ? trim($paramArray['ago']) : ''; ?>"></p>
           </div>
          
          <p>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true" <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
             <span class="clear"></span>
          </p>
          
            <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true)); ?> 
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
        </form>
    </div>
 </div>