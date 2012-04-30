<div id="searchform" class="panel">
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p><label>Scope or bundle: (.*+[])</label>
                <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"  <?php echo (is_array($paramArray) && isset($paramArray['hosts_only'])) ? 'checked' : ''; ?>></span>
                <span class="clear"></span>
                <input class="searchfield" type="text" name="scope" value="<?php echo (is_array($paramArray) && isset($paramArray['scope'])) ? trim($paramArray['scope']) : ''; ?>"></p>
                
            
                <p><label>Lvalue or name: (.*+[])</label><input class="searchfield" type="text" name="lval" value="<?php echo (is_array($paramArray) && isset($paramArray['lval'])) ? trim($paramArray['lval']) : ''; ?>"></p>
                <p><label>Rvalue or content: (.*+[])</label><input class="searchfield" type="text" name="rval" value="<?php echo (is_array($paramArray) && isset($paramArray['rval'])) ? trim($paramArray['rval']) : ''; ?>"></p>
                <p><label>Type:</label>
                    <select name="type">
                        <option value="s" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 's') ? 'selected' : ''; ?>>String</option>
                        <option value="sl" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 'sl') ? 'selected' : ''; ?>>String list</option>
                        <option value="i" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 'i') ? 'selected' : ''; ?>>Integer</option>
                        <option value="il" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 'il') ? 'selected' : ''; ?>>Integer list</option>
                        <option value="r" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 'r') ? 'selected' : ''; ?>>Real</option>
                        <option value="rl" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['type'] == 'rl') ? 'selected' : ''; ?>>Real list</option>
                    </select>
                     <span class="green_btn floatright" style="margin-top: 5px"><input class="green_btn" type="submit" value="Generate Report"></input></span>
                     <span class="clear"></span>
                </p>
            <?php $this->load->view('searchpages/hidden_fields_for_set_context'); ?>   
                
        </form>
    </div>
</div>