<div id="searchform" class="panel">
  <div class="panelhead"><?php echo $report_type?> query</div>
    <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Name: <input class="searchfield" type="text" name="name" id="packagename"></p>
          <p>Version:<input class="searchfield" type="text" name="version" size="80" id="version"></p>
          <p>Architecture:<input class="searchfield" type="text" name="arch" size="80" id="arc"></p>
          <p>Host group: (.*+[])
               <a href="<?php echo site_url('widget/allclasses')?>" id="bphghelp" class="help"
             title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
              <input class="searchfield" type="text" id="hg" name="class_regex" size="80"></p>
          <!--<input type="hidden" name="manyhosts" value="true">-->
          <input type="hidden" name="report" value="<?php echo $report_type?>">
           <input type="hidden" name="host" value="All">
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
        $('#hg').smartTextBox('add', data.selectedclass);
    }
});
</script>