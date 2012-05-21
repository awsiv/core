<div id="searchform" class="panel">
   <div class="panelcontent">
       <p>There are no filters for this report</p>
     <form id="weakest_hosts_filter" method="post" action="<?php echo site_url('search')?>">
      <?php $this->load->view('searchpages/buttons_for_filter'); ?>
      <?php $this->load->view('searchpages/hidden_fields_for_set_context');?>          
     </form>
   </div>
</div>