 <div id="searchform" class="panel">
   <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p><label>Package name: (.*+[])</label>
             <span class="floatright"> <label>Return host names only:</label> <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
             <span class="clear"></span>
              <input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          <p><label>Package version: (.*+[])</label><input class="searchfield" type="text" name="version" value="<?php echo (is_array($paramArray) && isset($paramArray['version'])) ? trim($paramArray['version']) : ''; ?>"></p>
          <p><label>Package architecture:</label><input class="searchfield" type="text" name="arch" value="<?php echo (is_array($paramArray) && isset($paramArray['arch'])) ? trim($paramArray['arch']) : ''; ?>"></p>
            <?php if ($report_type == "patches-installed") { ?>
                <div class="warning"><label><?php echo $this->lang->line('warning_only_suse'); ?></label></div>
            <?php } ?>
             <?php if ($report_type == "patches-available") { ?>
                <div class="warning"><label><?php echo $this->lang->line('warning_only_suse_redhat'); ?></label></div>
            <?php } ?>
            <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true)); ?>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>
          </form>
   </div>
 </div>