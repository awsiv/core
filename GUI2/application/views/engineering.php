<div id="body">
    <div class="outerdiv">
        <div id="hoststatus" class="minwidth30 innerdiv">
            <p class="title">Host Status (Last hour) </p>
            <ul>
                <li> <a href="<?php echo site_url('welcome/hosts/red') ?>" title="<?php echo tooltip('tooltip_redhost'); ?>" class="redhost showqtip hoststatuslist"><?php echo $r ?> hosts ( > 20% not compliant)</a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/yellow') ?>" title="<?php echo tooltip('tooltip_yellowhost'); ?>"  class="yellowhost showqtip hoststatuslist"><?php echo $y ?> hosts ( > 20% repaired now compliant) </a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/green') ?>" title="<?php echo tooltip('tooltip_greenhost'); ?>" class="greenhost showqtip hoststatuslist"><?php echo $g ?> hosts ( > 80% compliant) </a> </li>
                <li> <a href="<?php echo site_url('welcome/hosts/blue') ?>" title="<?php echo tooltip('tooltip_bluehost'); ?>" class="bluehost showqtip hoststatuslist"><?php echo $b ?> hosts unreachable</a> </li>
            </ul>

            <ul>
                <li>Hosts Known: <strong><?php echo $all ?></strong></li>
                <li><a href="weakest_host">Worst available host rank</a></li>
                <li><a href="<?php echo site_url() ?>/hubstatus/status" title="See the status of the hub replication" class="showqtip">Hub replication status</a></li>
            </ul>
        </div>
        <div id="summarymetereng" class="minwidth70 innerdiv">
            <p class="title">Promise compliance summary for reachable hosts</p>
            <div class="graphcontainer">
                <?php include_once('graph/summaryCompliance.php'); ?>
            </div>
        </div>
        <div class="clearboth"></div>


        <div id="finder" class="dash innerdiv">
            <p class="title">Finders</p>
            <ul>
                <li><?php echo anchor('#', 'host', array('class' => 'host showqtip', 'title' => tooltip('tooltip_host_finder'), 'id' => 'findhost')); ?></li>
                <li><?php echo anchor('widget/allclasses', 'class', array('class' => 'classes showqtip', 'title' => tooltip('tooltip_class_finder'), 'id' => 'findclass')); ?></li>
                <li><?php echo anchor('widget/allpolicies', 'promises', array('class' => 'policies showqtip', 'title' => tooltip('tooltip_promise_finder'), 'id' => 'findpolicy')); ?></li>
                <li><?php echo anchor('widget/allreports', 'reports', array('class' => 'report showqtip', 'id' => 'findreport', 'title' => tooltip('tooltip_reports_finder'))); ?></li>
                <li><?php echo anchor('summaryreports/search', 'summary reports', array('class' => 'summary_report showqtip', 'id' => 'summaryreport', 'title' => tooltip('tooltip_summary_finder'))); ?></li>
                <li><?php echo anchor('widget/cdpreports', 'CDP reports', array('class' => 'cdpreport showqtip', 'id' => 'findcdpreport', 'title' => tooltip('tooltip_reports_finder'))); ?></li>

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
            url: "<?php echo site_url() ?>/widget/hostfinder",
            classhandler:"<?php echo site_url() ?>/widget/cfclasses"};
    
        $('#findhost').hostfinder(hostFinderOption);
        $('#findclass').classfinder(genericOption);
        $('#findpolicy').policyfinder(genericOption);
        $('#findreport').reportfinder(genericOption);
        $('#summaryreport').ajaxyDialog({'title':'Summary report',dontOverrideTitle:true,
            'width':'50%'});
        $('#findcdpreport').cdpreportfinder(genericOption);
        
        //fix for manintaining the layout of the page when window is resized.
        $('#compliance_summary').css('width','50px');
        
        
        
        
        
    });
</script>