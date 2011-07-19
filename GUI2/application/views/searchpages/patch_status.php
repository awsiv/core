 <div id="searchform" class="panel">
   <div class="panelhead"><?php echo $report_type ?></div>
   <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Package name: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>package version: (.*+[])<input class="searchfield" type="text" name="version"></p>
          <p>Package architecture: (.*+[])<input class="searchfield" type="text" name="arch"></p>
          <p>Host group: (.*+[])
            <a href="<?php echo site_url('widget/allclasses')?>" id="bphghelp" class="help"
             title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
          <input class="searchfield" type="text" name="class_regex" size="80" id="hg"></p>
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

 $('#hg').smartTextBox({separator : "|"});
$('#bphghelp').classfinder({
    defaultbehaviour:false,
    complete:function(event,data){
        //console.log(data.selectedclass);
        $('#hg') .smartTextBox('add', data.selectedclass);
    }
});
</script>