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

                <p>
                    Time 

                    <?php
                    $options = array('h' => 'Hour',
                        'd' => 'Day',
                        'w' => 'Week');

                    echo form_dropdown('time', $options, $this->input->post('time'));
                    ?>


                </p>
                <div style="display: none;" id="showSelectedHostContainer"><p>Selected host :: <span id="showSelectedHost"></span><span> ( <a href="#" target="_self"  id="removeshowSelectedHost"> x </a>  )  </span></p></div>
                <p><?php echo anchor('#', 'Select a host', array('id' => 'findhostforsummary', 'title' => 'Report for another host', 'class' => 'slvbutton', 'target' => '_self')) ?></p>
                <input type="hidden" name="host" id="host" value="<?php echo set_value('host', ''); ?>">
                <input type="hidden" name="report" value="Summary report query">
                <p></p>
                <p><input class="btn" type="submit" value="Generate report"></p>
            </form>
        </div>
    </div>
</div>    
<script type="text/javascript">
    $('#hg').smartTextBox({separator : "|"});
    $('#handlebox').smartTextBox({separator : "|"});
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
    
    
    
    
    $('#removeshowSelectedHost').click(function(e) {
        // remove the text remove the value from hidden field.
        e.preventDefault();
        $('#host').val('');
        $('#showSelectedHost').html('');
        $('#showSelectedHostContainer').hide();
        return false;
    });

    $('#findhostforsummary').hostfinder({
        'defaultbehaviour':false,
            
        complete:function(event,data){
            var instance = $(this).data("hostfinder");
            var text = data.selectedHostName;
            var hostId = data.selectedhost;
            $('#host').val(hostId);
            $('#showSelectedHost').html(text).show();
            $('#showSelectedHostContainer').show();
                
            instance.hideDialog();
            return false;
        }

    });

</script>