 <div id="searchform" class="panel">
   <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p><label>Package name: (.*+[])</label><input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          <div id="morefilters" style="display:none">
          <p><label>Package version: (.*+[])</label><input class="searchfield" type="text" name="version" value="<?php echo (is_array($paramArray) && isset($paramArray['version'])) ? trim($paramArray['version']) : ''; ?>"></p>
          <p><label>Package architecture:</label><input class="searchfield" type="text" name="arch" value="<?php echo (is_array($paramArray) && isset($paramArray['arch'])) ? trim($paramArray['arch']) : ''; ?>"></p>
          </div>
              
          <p>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
         </p>
         
         <p class="alignrightbuttoncontainer">
             <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
             <a  id="morefiltertoggle"class="green_btn" href="#"><span>More Filters</span></a>
         </p>
            <?php $this->load->view('searchpages/hidden_form_for_set_context'); ?>   
          </form>
   </div>
 </div>
<script type="text/javascript">
$('#morefiltertoggle').live('click',function(){
     var textcontainer=$(this).find('span');
    if( textcontainer.text().toLowerCase()=='more filters'){
       textcontainer.text('Fewer Filters') 
    }else {
        textcontainer.text('More Filters') 
    }
     $('#morefilters').toggle();
});
</script>
