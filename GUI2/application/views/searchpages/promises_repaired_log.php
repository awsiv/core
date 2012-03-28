<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search')?>">
        <p>Promise handles: (.*+[])<input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>

        <div id="morefilters" style="display:none">
            <p>
                Cause <input type="text" name="cause" class="searchfield" value="<?php echo (is_array($paramArray) && isset($paramArray['cause'])) ? trim($paramArray['cause']) : ''; ?>">
            </p>
            <p>
                From minimum age <input type="text" name="hours_deltafrom" size="2" value="<?php echo (is_array($paramArray) && isset($paramArray['hours_deltafrom'])) ? trim($paramArray['hours_deltafrom']) : ''; ?>">
                hours to maximum age <input type="text" name="hours_deltato" size="2" value="<?php echo (is_array($paramArray) && isset($paramArray['hours_deltato'])) ? trim($paramArray['hours_deltato']) : ''; ?>"> hours old.
            </p>
            
        </div>

        <p>
            <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
            <span class="clear"></span>
        </p>

        <?php $this->load->view('searchpages/buttons_for_filter', array('morefilters' => true)); ?> 
        <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
        </form>
    </div>
</div>