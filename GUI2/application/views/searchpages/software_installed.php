<div id="searchform" class="panel">
    <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
           <p><label>Name:</label> 
                <span class="floatright"><label> Return host names only: </label><input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                <span class="clear"></span>
               <input class="searchfield" type="text" name="name" id="packagename" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          
         
            <p><label>Version:</label><input class="searchfield" type="text" name="version" size="80" id="version" value="<?php echo (is_array($paramArray) && isset($paramArray['version'])) ? trim($paramArray['version']) : ''; ?>"></p>
            <p><label>Architecture:</label><input class="searchfield" type="text" name="arch" size="80" id="arc" value="<?php echo (is_array($paramArray) && isset($paramArray['arch'])) ? trim($paramArray['arch']) : ''; ?>"></p>
              
        <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true)); ?> 
        <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
          </form>
     </div>
  </div>