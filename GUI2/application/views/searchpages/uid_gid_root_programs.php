 <div id="searchform" class="panel">
     <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Filename: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
         </p>
        <?php $this->load->view('searchpages/buttons_for_filter'); ?> 
        <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
          </form>
      </div>
</div>