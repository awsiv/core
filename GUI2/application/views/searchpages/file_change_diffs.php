<div id="searchform" class="panel"><div class="panelhead"><?php echo $report_type?> query</div><div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>File name: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>Match content: (.*+[])<input class="searchfield" type="text" name="diff" size="80"></p>
          <p>Host group: (.*+[])
               <a href="<?php echo site_url('widget/allclasses')?>" id="bphghelp" class="help"
             title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
              <input class="searchfield" type="text" name="class_regex" size="80" id="hg"></p>
          <!--<input type="hidden" name="manyhosts" value="true">-->
          <input type="hidden" name="host" value="All">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input class="btn" type="submit" value="Generate report"></p>
          </form>
  </div>
</div>
<script type="text/javascript">
 $('#hg').smartTextBox({separator : "|"});
$('#bphghelp').classfinder({
    defaultbehaviour:false,
    complete:function(event,data){
        //console.log(data.selectedclass);
        $('#hg') .smartTextBox('add', data.selectedclass);
    }
});
$('#bphghelp').qtip({
            style: {
                background:'#454545',
                opacity:0.9,
                name: 'dark',
                width:{
                    min:0,
                    max:150
                },
                border: {
                    width: 1,
                    radius: 4
                },
                tip:{
                    corner:'topMiddle',
                    size:{
                        x:10,
                        y:10
                    }
                }
            },
            show:{
                effect:{
                    type:'fade',
                    lenght:200
                }
            },

            position: {
                corner: {
                    target: 'bottomMiddle',
                    tooltip: 'topMiddle'
                }
            }
        });
</script>