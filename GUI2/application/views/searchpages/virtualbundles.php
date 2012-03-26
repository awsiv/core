
<div id="searchform" class="panel">
    <div class="panelhead"><?php echo $report_type . ' query' ?> </div>
    <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>Virtual bundle pattern: (.*+[])<input class="searchfield" type="text" name="name" id="bp" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>" ></p>
            <p>Show definitions from all users: <input type="checkbox" name="all_user" value="true" <?php echo (is_array($paramArray) && isset($paramArray['all_user']) && $paramArray['all_user'] == 'true') ? 'checked' : ''; ?>></p>
            <p><input class="btn" type="submit" value="Generate report">  <span>       
                    <a href="<?php echo site_url("/savedsearch/listSavedSearches/$report_type"); ?>"  class="btn loadsavedsearch">Load saved searches</a>
                </span>  </p>
            <input type="hidden" name="host" value="All">
            <input type="hidden" name="report" value="<?php echo $report_type ?>">
        </form>
    </div>

</div>
<script type="text/javascript">
    $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});
    //$('#hg').smartTextBox({separator : "|",editOnFocus:true});
    $('#bphghelp').classfinder({
        defaultbehaviour:false,
        complete:function(event,data){
            var val=$('#hg').val()
            if(val==""){
                $('#hg').val(data.selectedclass);
            }else{
                $('#hg').val(val+'|'+data.selectedclass);
            }
        }
    });
</script>
