<div id="searchform" class="panel">
   <div class="panelcontent">
     <form method="post" action="<?php echo site_url('search')?>">
         <p>Bundle pattern: (.*+[])<input class="searchfield" type="text" name="name" id="bp" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
         <p>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
         </p>
         <p class="alignrightbuttoncontainer">
             <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
         </p>
      <?php $this->load->view('searchpages/hidden_form_for_set_context');?>          
     </form>
   </div>
</div>