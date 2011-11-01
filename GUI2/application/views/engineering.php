<div id="body">
    <div class="outerdiv">
        <div id="hoststatus" class="minwidth30 innerdiv">
            <p class="title"><?php echo $this->lang->line('header_host_status'); ?></p>
            <ul>
                <li> <a href="<?php echo site_url('welcome/hosts/red') ?>" title="<?php echo tooltip('tooltip_redhost'); ?>" class="redhost showqtip hoststatuslist"><?php echo $r ?> <?php echo $this->lang->line('not_compliant'); ?></a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/yellow') ?>" title="<?php echo tooltip('tooltip_yellowhost'); ?>"  class="yellowhost showqtip hoststatuslist"><?php echo $y ?> <?php echo $this->lang->line('now_compliant'); ?> </a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/green') ?>" title="<?php echo tooltip('tooltip_greenhost'); ?>" class="greenhost showqtip hoststatuslist"><?php echo $g ?> <?php echo $this->lang->line('compliant'); ?> </a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/blue') ?>" title="<?php echo tooltip('tooltip_bluehost'); ?>" class="bluehost showqtip hoststatuslist"><?php echo $b ?> <?php echo $this->lang->line('unreachable'); ?></a> </li>
            </ul>

            <ul>
                <li><?php echo $this->lang->line('host_known'); ?>: <strong><?php echo $all ?></strong></li>
                <li><a href="weakest_host"><?php echo $this->lang->line('worst_host'); ?></a></li>
                <li><a href="<?php echo site_url() ?>/hubstatus/status" title="See the status of the hub replication" class="showqtip"><?php echo $this->lang->line('hub_replication_status'); ?></a></li>
            </ul>
        </div>
        <div id="summarymetereng" class="minwidth70 innerdiv">
            <p class="title"><?php echo $this->lang->line('header_promise_compliance'); ?></p>
            <div class="graphcontainer">
                <?php include_once('graph/summaryCompliance.php'); ?>
            </div>
        </div>
        <div class="clearboth"></div>


        <div id="finder" class="dash innerdiv">
            <p class="title"><?php echo $this->lang->line('header_finders'); ?></p>
            <ul>
                <li><?php echo anchor('#',$this->lang->line('host'), array('class' => 'host showqtip', 'title' => tooltip('tooltip_host_finder'), 'id' => 'findhost')); ?></li>
                <li><?php echo anchor('widget/allclasses', $this->lang->line('class'), array('class' => 'classes showqtip', 'title' => tooltip('tooltip_class_finder'), 'id' => 'findclass')); ?></li>
                <li><?php echo anchor('widget/allpolicies', $this->lang->line('promises'), array('class' => 'policies showqtip', 'title' => tooltip('tooltip_promise_finder'), 'id' => 'findpolicy')); ?></li>
                <li><?php echo anchor('widget/allreports', $this->lang->line('reports'), array('class' => 'report showqtip', 'id' => 'findreport', 'title' => tooltip('tooltip_reports_finder'))); ?></li>
                <li><?php echo anchor('summaryreports/search', $this->lang->line('summary_reports'), array('class' => 'summary_report showqtip', 'id' => 'summaryreport', 'title' => tooltip('tooltip_summary_finder'))); ?></li>
                <li><?php echo anchor('widget/cdpreports', $this->lang->line('cdp_reports'), array('class' => 'cdpreport showqtip', 'id' => 'findcdpreport', 'title' => tooltip('tooltip_reports_finder'))); ?></li>
                <p class="clearleft"></p>
            </ul>

        </div>
    </div>
</div>
<div class="clear"></div>

<script type="text/javascript">
    $(document).ready(function(){
        
        var genericOption = {
            baseUrl: '<?php echo site_url() ?>'
        };
        
        var hostFinderOption =   {
            baseUrl:'<?php echo site_url() ?>'
          };
    
        $('#findhost').hostfinder(hostFinderOption);
        $('#findclass').classfinder(genericOption);
        $('#findpolicy').policyfinder(genericOption);
        $('#findreport').reportfinder(genericOption);
        $('#summaryreport').ajaxyDialog({'title':'Summary report',dontOverrideTitle:true,
            'width':'50%'});
        $('#findcdpreport').cdpreportfinder(genericOption);
        //for constellation
        $('#findvbundle').ajaxyDialog({'title':'Virtual bundle',dontOverrideTitle:true,
            'width':'50%'});
        //fix for manintaining the layout of the page when window is resized.
        $('#compliance_summary').css('width','50px');
        
        
        
        
        
    });
</script>