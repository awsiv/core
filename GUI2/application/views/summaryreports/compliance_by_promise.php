<div  style="display: block;min-height: 400; background-color: #3D453A;color:#FFFFFF">
    <div id="searchform" class="panel"><div class="panelhead">Summary report query</div>
        <div class="panelcontent">
            <form method="post" target="_self" action="<?php echo site_url('summaryreports/show/') ?>">
                <p>Promise by handle: (.*+[])    <a target="_self" href="<?php echo site_url('widget/allpolicies') ?>" id="bphghelp-promise" class="help"
                                                    title="<?php echo $this->lang->line('report_hostgp_help'); ?>">Help ?</a>

                    <input class="searchfield" id="handlebox" type="text" name="name" value="<?php echo set_value('name', ''); ?>"></p>

                <p>Host group: (.*+[])     <a target="_self" href="<?php echo site_url('widget/allclasses') ?>" id="bphghelp" class="help"
                                              title="<?php echo $this->lang->line('report_hostgp_help'); ?>">Help ?</a>
                    <input class="searchfield" type="text" name="class_regex" size="80" id="hg" value="<?php echo set_value('class_regex', ''); ?>"></p>



                <p>Select a host:    <a target="_self" href="#" id="bphghelp-host" class="help"
                                        title="<?php echo $this->lang->line('report_hostgp_help'); ?>">Select</a>

                    <input class="searchfield" id="hostbox" type="text" readonly="readonly" name="hostbox" value="<?php echo set_value('hostbox', ''); ?>">
                     <input class="searchfield" id="host" type="hidden"  name="host" value="<?php echo set_value('host', ''); ?>">
                
                </p>                
                <input type="hidden" name="report" value="Summary report query">
                <p><input class="btn" type="submit" value="Generate report"></p>
            </form>
        </div>
    </div>
</div>    
<script type="text/javascript">
    $('#hg').smartTextBox({separator : "|"});
    $('#handlebox').smartTextBox({separator : "|"});
    $('#hostbox').smartTextBox({separator : ",",maxResults:1,uniqueValues:true,onElementRemove:function(){
              $('#host').val(''); // remove the hidden val as well
    }});
    $('#bphghelp').classfinder({
        defaultbehaviour:false,
        complete:function(event,data){
            $('#hg') .smartTextBox('add', data.selectedclass);
        }
    });
    
    $('#bphghelp-promise').policyfinder({
        defaultbehaviour:false,
        onlyShowHandle:true,
        handleClicked:function(event,data){
            $('#handlebox').smartTextBox('add', data.selectedHandleName);
            var instance = $(this).data("policyfinder");
            instance.hideDialog();
            
        }
    });
    
    
    
    

    $('#bphghelp-host').hostfinder({
        'defaultbehaviour':false,
            
        complete:function(event,data){
            var instance = $(this).data("hostfinder");
            var text = data.selectedHostName;
            var hostId = data.selectedhost;
             $('#hostbox').smartTextBox('clear');
             $('#hostbox').smartTextBox('add', text);
              $('#host').val(hostId);
            
            instance.hideDialog();
            return false;
        }

    });

</script>