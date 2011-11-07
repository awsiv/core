<div id="searchform" class="panel">
    <div class="panelhead"><?php echo $report_type ?> query</div>
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>Scope or bundle: (.*+[])<input class="searchfield" type="text" name="scope" value="<?php echo (is_array($paramArray) && isset($paramArray['scope'])) ? trim($paramArray['scope']) : ''; ?>"></p>
            <p>Lvalue or name: (.*+[])<input class="searchfield" type="text" name="lval" value="<?php echo (is_array($paramArray) && isset($paramArray['lval'])) ? trim($paramArray['lval']) : ''; ?>"></p>
            <p>Rvalue or content: (.*+[])<input class="searchfield" type="text" name="rval" value="<?php echo (is_array($paramArray) && isset($paramArray['rval'])) ? trim($paramArray['rval']) : ''; ?>"></p>
            <p>Type:<p>
                <select name="type">
                    <option value="s" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 's') ? 'selected' : ''; ?>>String</option>
                    <option value="sl" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 'sl') ? 'selected' : ''; ?>>String list</option>
                    <option value="i" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 'i') ? 'selected' : ''; ?>>Integer</option>
                    <option value="il" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 'il') ? 'selected' : ''; ?>>Integer list</option>
                    <option value="r" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 'r') ? 'selected' : ''; ?>>Real</option>
                    <option value="rl" <?php echo (is_array($paramArray) && isset($paramArray['type']) && $paramArray['state'] == 'rl') ? 'selected' : ''; ?>>Real list</option>
                </select>
            <p>Host class: (.*+[])
                <a href="<?php echo site_url('widget/allclasses') ?>" id="bphghelp" class="help"
                   title="<?php echo $this->lang->line('report_hostgp_help'); ?>">Help ?</a>
                <input class="searchfield" type="text" name="class_regex" size="80" id="hg" value="<?php echo (is_array($paramArray) && isset($paramArray['class_regex'])) ? trim($paramArray['class_regex']) : ''; ?>"></p>
            <input type="hidden" name="host" value="All">
            <input type="hidden" name="report" value="<?php echo $report_type ?>">
            <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
            <p><input class="btn" type="submit" value="Generate report"> <span>       <a href="<?php echo site_url("/savedsearch/listSavedSearches/$report_type"); ?>"  class="btn loadsavedsearch">Load saved searches</a>
                </span> </p>
        </form>
    </div>
</div>
<script type="text/javascript">
    $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});

    //$('#hg').smartTextBox({separator : "|",editOnFocus:true});
    $('#bphghelp').classfinder({
        defaultbehaviour:false,
        complete:function(event,data){
            //console.log(data.selectedclass);
            //$('#hg') .smartTextBox('add', data.selectedclass);
               var val=$('#hg').val()
        if(val==""){
             $('#hg').val(data.selectedclass);
        }else{
            $('#hg').val(val+'|'+data.selectedclass);
        }
        }
    });
</script>
