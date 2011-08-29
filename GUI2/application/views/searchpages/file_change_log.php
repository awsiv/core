<div id="searchform" class="panel"><div class="panelhead"><?php echo $report_type?> query</div><div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>File name: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>Host group: (.*+[])
               <a href="<?php echo site_url('widget/allclasses')?>" id="bphghelp" class="help"
             title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
              <input class="searchfield" type="text" name="class_regex" size="80" id="hg"></p>
          <!--<p><input type="hidden" name="manyhosts" value="true"></p>-->
          <p><input type="hidden" name="host" value="All"></p>
          <p><input type="hidden" name="report" value="<?php echo $report_type?>"></p>
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p>Show long term data: <input type="checkbox" name="long_term" value="true" /></p>
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