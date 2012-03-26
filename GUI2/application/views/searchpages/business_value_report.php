
<div id="searchform" class="panel">
    <div class="panelcontent searchform">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>
                <label>Date:</label>
                <input class="searchfield" type="text" name="date" value="<?php echo (is_array($paramArray) && isset($paramArray['date'])) ? trim($paramArray['date']) : ''; ?>">
            </p>
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
<script type="text/javascript">
$( "input:text[name=date]" ).datepicker({ 
        dateFormat: 'dd M yy', 
        changeMonth: true,
        changeYear: true,
        showButtonPanel: true
 });
</script>
