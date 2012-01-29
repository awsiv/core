<?php
$last = cfpr_getlastupdate($hostkey);
$class = cfpr_get_variable($hostkey, "sys", "ostype");
$flavour = cfpr_get_variable($hostkey, "sys", "flavour");
$rel = cfpr_get_variable($hostkey, "sys", "release");
$load = cfpr_get_variable($hostkey, "mon", "av_loadavg");
$free = cfpr_get_variable($hostkey, "mon", "av_diskfree");
$speed = cfpr_get_network_rate($hostkey);
$colour = cfpr_get_host_colour($hostkey);
?>
<div id="hostview">
    <div class="outerdiv">
        <div class="grid_4">
            <div id="hostviewerbtn" class="dash innerdiv">
                <p class="title"> <img src= "<?php echo get_imagedir() . $colour ?>_square_little.png" class="hoststatcolorsquare"/><?php echo $this->lang->line('header_action'); ?></p>
                <ul>
                    <li><?php echo anchor('widget/allreports', $this->lang->line('host_reports'), array('id' => 'findreport', 'title' => 'Generate report for this host', 'class' => 'report showqtip')) ?></li>
                    <?php
                    if ($is_commented != "") {
                        $noteUrl = site_url('notes/index/action/show/nid') . '/' . $is_commented . '/';
                    } else {
                        $noteUrl = site_url('notes/index/action/add/hostkey') . '/' . $hostkey . '/';
                    }
                    ?>
                    <li><a href="<?php echo $noteUrl ?>" class="note showqtip" id="add_cmt" title="Add note about this host"><?php echo $this->lang->line('host_notes'); ?></a></li>
                    <li> <?php echo anchor('visual/vital/' . $hostkey, $this->lang->line('host_vitals'), array('id' => 'pulseNvitals', 'class' => 'pulseNvitals showqtip', 'title' => 'View probes regarding cpu, memory, network etc')) ?></li>
                    <li><a href="#" id="notkeptlnk" title="Promises not kept in the past week" class="problems showqtip"><?php echo $this->lang->line('host_problems'); ?></a></li>
                    <p class="clearleft"></p>
                </ul>
            </div>
            <div id="monitoredjob"class="innerdiv">
                <p class="title">  <img src= "<?php echo get_imagedir() . $colour ?>_square_little.png" class="hoststatcolorsquare"/><?php echo $this->lang->line('header_monitor_job'); ?></p>
                <?php
                $number = cfpr_get_class_frequency(NULL, "mXC.*");
                $count = 0;
                $nlist = json_decode(cfpr_report_class_frequency($hostkey, "mXC.*"), true);
//  echo cfpr_report_class_frequency($hostkey,"mXC.*");
                ?>
                <p><?php echo $this->lang->line('host_total_surveillance') ?>: <?php echo $number ?><p>
                <p><?php echo $this->lang->line('host_on_host') ?>: <?php
                if (!empty($nlist)) {
                    foreach ((array) $nlist as $list) {
                        if ($list['count'] > 0) {
                            echo "<p>" . $list['name'] . ": " . $list['count'] . "</p>";
                        } else {
                            echo $count;
                            break;
                        }
                    }
                } else {
                    echo "no data found";
                }
                ?></p> <!--proble, with varaibale nlist-->

            </div>

            <div id="statusmeasured" class="innerdiv ">
                <p class="title"> <img src= "<?php echo get_imagedir() . $colour ?>_square_little.png" class="hoststatcolorsquare"/><?php echo $this->lang->line('header_status'); ?></p>

                <p><label class="width_20"><?php echo $this->lang->line('host_average_load') ?>: </label><label ><?php echo $load ?>%</label></p>
                <p> <label class="width_20"><?php echo $this->lang->line('host_free_disk') ?>: </label><label><?php echo $free ?>%</label></p>
                <p><label class="width_20"><?php echo $this->lang->line('host_network_speed') ?>: </label><label><?php echo $speed ?></label></p>

            </div>
        </div>
        <div id="hostviewerrightpanes" class="grid_8">
            <div class="innerdiv">
                <p class="title"> <img src= "<?php echo get_imagedir() . $colour ?>_square_little.png" class="hoststatcolorsquare"/><?php echo $this->lang->line('header_analysis'); ?></p>
                <div id="hostviewgraph" class="graphcontainer"><?php include_once('graph/summaryCompliance.php'); ?></div>
            </div>
            <div id="hostinformation" class="innerdiv">


                <p class="title"> <img src= "<?php echo get_imagedir() . $colour ?>_square_little.png" class="hoststatcolorsquare"/><?php echo $this->lang->line('header_host_details'); ?></p>
                <form  id="delform"method="post" action="/welcome/host">
                    <input type="hidden" name="delhost" id="delhost" value="<?php echo $hostkey ?>"/>
                    <input class="btn"  type="submit" id="btnsubmit"  value="<?php echo $this->lang->line('host_delete'); ?>"/>
                </form>
                <br class="clear">
                <p><label><?php echo $this->lang->line('host_alias'); ?>: </label><label ><?php echo ($hostname == "") ? $this->lang->line('host_not_discovered') : $hostname ?></label></p>
                <p><label><?php echo $this->lang->line('host_os_class'); ?>: </label><label><?php echo ($class == "") ? $this->lang->line('host_not_discovered') : $class ?></label></p>
                <p><label><?php echo $this->lang->line('host_release'); ?>: </label><label><?php echo ($rel == "") ? $this->lang->line('host_not_discovered') : $rel ?></label></p>
                <p><label><?php echo $this->lang->line('host_flavour'); ?>: </label><label><?php echo ($flavour == "") ? $this->lang->line('host_not_discovered') : $flavour ?></label></p>
                <p><label><?php echo $this->lang->line('host_ip'); ?>: </label><label><?php echo ($ipaddr == "") ? $this->lang->line('host_not_discovered') : $ipaddr ?></label></p>
                <p><label><?php echo $this->lang->line('host_last_data'); ?>: </label><label><?php echo getDateStatus($last); ?></label></p>
                <p><label><?php echo $this->lang->line('host_id'); ?>: </label><label><small><?php echo $hostkey ?></small></label></p>
            </div>
        </div>
        <div class="clear"></div>
        <div id="notkepttable" title="<?php echo $this->lang->line('host_promise_not_kept'); ?>">
            <?php $report = cfpr_summarize_notkept($this->session->userdata('username'), $hostkey, NULL, NULL, NULL, NULL, NULL, NULL); ?>
               <!--<p class="title">Promises not kept in the past week</p-->
            <div class="bundlelist-table">
                <?php
                $tableData = json_decode($report, true);
                if (is_array($tableData)) {
                    echo $this->cf_table->generateReportTable($tableData);
                } else {
                    echo '<span class=\"nodata\">' . $this->lang->line('no_data') . "</span>";
                }
                ?>
            </div>
        </div>
    </div>
</div>
<div id="hostdelconfirmation" title="<?php echo $this->lang->line('header_host_delete'); ?>">
    <span>
        <?php echo $this->lang->line('host_delete_confirm'); ?>
    </span>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        // $('.tables table').tableFilter();
        //$('.tables table').tablesorter({widgets: ['zebra']});

        $('.note').ajaxyDialog({change:function(nid,element) {

                // change the url
                $(element).attr('href', '/notes/index/action/show/nid/' + nid);
                // console.log(nid,element);
            },title:'Add note about this host',dontOverrideTitle:true});
    
        $('#findreport').reportfinder({allhost:false,hostkey:"<?php echo $hostkey; ?>",baseUrl: '<?php echo site_url() ?>'});

        var $notkeptbox = $('#notkepttable').dialog({
            autoOpen: false,
            modal: true,
            width: 'auto',
            buttons: {
                'Ok': function() {
                    $(this).dialog('close');
                }
            },
            open: function() {
                //$(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus()
            }
        });
         
        $('#notkeptlnk').bind('click',function(event){
            event.preventDefault();
            $notkeptbox.dialog('open');
        });

        var $hostdelconfirm = $('#hostdelconfirmation').dialog({
            autoOpen: false,
            modal: true,
            width: 400,
            resizable:false,
            buttons: {
                'Yes': function() {
                    $(this).dialog('close');
                    $("#delform").submit();
                },
                'NO': function() {
                    $(this).dialog('close');
                }
            },
            open: function() {
                //$(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus()

            }
        });

        /*$("#delform").submit(function(event){
        event.preventDefault();
        $hostdelconfirm.dialog('open');
    });*/

        $("[type=submit]").bind("click", function(e){
            e.preventDefault();
            $hostdelconfirm.dialog('open');
        });
    });
</script>
