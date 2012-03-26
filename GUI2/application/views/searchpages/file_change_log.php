<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>File name: (.*+[])<input class="searchfield" type="text" name="name" title="Regular expression supported" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
            <div id="morefilters" style="display:none">
                <p>Show long term data: <input type="checkbox" name="long_term" value="true" <?php echo (is_array($paramArray) && isset($paramArray['long_term']) && $paramArray['long_term'] == 'true') ? 'checked' : ''; ?> /></p>
            </div>
            <p>
                <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
                <span class="clear"></span>
            </p>
            <p class="alignrightbuttoncontainer">
                <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
                <a  id="morefiltertoggle"class="green_btn" href="#"><span>More Filters</span></a>
            </p>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>    
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
