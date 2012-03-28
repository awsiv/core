
<div id="searchform" class="panel">
    <div class="panelcontent searchform">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>
                <label>Date:</label>
                <input class="searchfield" type="text" name="date" value="<?php echo (is_array($paramArray) && isset($paramArray['date'])) ? trim($paramArray['date']) : ''; ?>">
            </p>
            <p>
                <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                <span class="clear"></span>
            </p>
            <?php $this->load->view('searchpages/buttons_for_filter');?>     
            <?php $this->load->view('searchpages/hidden_fields_for_set_context');?>     
        </form>
    </div>
</div>
<script type="text/javascript">
$( "input:text[name=date]" ).datepicker({ 
        dateFormat: 'dd M yy', 
        changeMonth: true,
        changeYear: true,
        showButtonPanel: true
 });
</script>
