 <div id="searchform" class="panel">
     <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Job name: (.*+[])<input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          <p>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
         </p>
          <p class="alignrightbuttoncontainer">
             <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
            
         </p>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
          </form>
    </div>
 </div>