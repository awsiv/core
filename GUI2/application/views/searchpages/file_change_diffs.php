<div id="searchform" class="panel"><div class="panelhead"><?php echo $report_type?> query</div><div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>File name: (.*+[])<input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          <p>Match content: (.*+[])<input class="searchfield" type="text" name="diff" size="80" value="<?php echo (is_array($paramArray) && isset($paramArray['diff'])) ? trim($paramArray['diff']) : ''; ?>"></p>
          <p>Host class: (.*+[])
               <a href="<?php echo site_url('widget/allclasses')?>" id="bphghelp" class="help"
             title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
              <input class="searchfield" type="text" name="class_regex" size="80" id="hg" value="<?php echo (is_array($paramArray) && isset($paramArray['class_regex'])) ? trim($paramArray['class_regex']) : ''; ?>"></p>
          <!--<input type="hidden" name="manyhosts" value="true">-->
          <input type="hidden" name="host" value="All">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p>Show long term data: <input type="checkbox" name="long_term" value="true" <?php echo (is_array($paramArray) && isset($paramArray['long_term']) && $paramArray['long_term'] == 'true') ? 'checked' : ''; ?> /></p>
          <p><input class="btn" type="submit" value="Generate report">  <span>       <a href="<?php echo site_url("/savedsearch/listSavedSearches/$report_type"); ?>"  class="btn loadsavedsearch">Load saved searches</a>
</span> </p>
          </form>
  </div>
</div>
<script type="text/javascript">
    $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});

 //$('#hg').smartTextBox({separator : "|",editOnFocus:true});
$('#bphghelp').classfinder({
    defaultbehaviour:false,
    baseUrl:"<?php echo base_url();?>",
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
