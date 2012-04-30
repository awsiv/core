<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>Promise by handle: (.*+[])
               <span class="floatright"> <label>Return host names only:</label> <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
               <span class="clear"></span>
               <input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>">
            </p>
             <p>
                    <label>Promises on status</label>
                    <select name="state"><option value="x" <?php echo (is_array($paramArray) && isset($paramArray['state']) && $paramArray['state'] == 'x') ? 'selected' : ''; ?>>Any</option>
                        <option value="c" <?php echo (is_array($paramArray) && isset($paramArray['state']) && $paramArray['state'] == 'c') ? 'selected' : ''; ?>>Compliant</option>
                        <option value="r" <?php echo (is_array($paramArray) && isset($paramArray['state']) && $paramArray['state'] == 'r') ? 'selected' : ''; ?>>Repaired</option>
                        <option value="n" <?php echo (is_array($paramArray) && isset($paramArray['state']) && $paramArray['state'] == 'n') ? 'selected' : ''; ?>>Non-compliant</option>
                    </select>
                    <span class="green_btn floatright" style="margin-top: 5px"><input class="green_btn" type="submit" value="Generate Report"></input></span>
                    <span class="clear"></span>
             </p>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
        </form>
    </div>
</div>