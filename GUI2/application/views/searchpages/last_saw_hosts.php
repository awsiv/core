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
