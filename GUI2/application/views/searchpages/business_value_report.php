
<div id="searchform" class="panel">
    <div class="panelhead"><?php echo $report_type ?> query</div>
    
    <div class="panelcontent searchform">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>Date:<input class="searchfield" type="text" name="days" value="<?php echo (is_array($paramArray) && isset($paramArray['days'])) ? trim($paramArray['days']) : ''; ?>" ></p>
            <p>Host class: (.*+[])
                <a href="<?php echo site_url('widget/allclasses') ?>" id="bphghelp" class="help" 
                   title="<strong>SOFT CLASSES</strong> or <strong> HARD CLASSES</strong> 
                   with combination of regular expression as input, To open <strong>classfinder</strong> click me!">Help ?</a>
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

   // $('#hg').smartTextBox({separator : "|",editOnFocus:true});
    $('#bphghelp').classfinder({
        defaultbehaviour:false,
        complete:function(event,data){
           
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
