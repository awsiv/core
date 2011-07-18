<div id="body">
    <div class="outerdiv">
        <div id="hoststatus" class="minwidth30 innerdiv">
         <p class="title">Host Status (Last hour) </p>
        <ul>
            <li> <a href="<?php echo site_url('welcome/hosts/red')?>" title="<?php echo lang('tooltip_redhost'); ?>" class="redhost showqtip hoststatuslist"><?php echo $r ?> hosts 20% failure</a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/yellow')?>" title="<?php echo lang('tooltip_yellowhost'); ?>"  class="yellowhost showqtip hoststatuslist"><?php echo $y ?> hosts at 20% repaired </a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/green')?>" title="<?php echo lang('tooltip_greenhost'); ?>" class="greenhost showqtip hoststatuslist"><?php echo $g ?> hosts greater than 80% compliant </a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/blue')?>" title="<?php echo lang('tooltip_bluehost'); ?>" class="bluehost showqtip hoststatuslist"><?php echo $b ?> hosts unreachable</a> </li>
        </ul>

            <ul>
                <li>Hosts Known: <strong><?php echo $all ?></strong></li>
                <li>Ranked worst: <a href="weakest_host">Show top 50</a></li>
            </ul>
        </div>
        <div id="summarymetereng" class="minwidth70 innerdiv">
            <p class="title">Promise compliance summary for all host(s)</p>
            <div class="graphcontainer">
                <?php include_once('graph/summaryCompliance.php'); ?>
            </div>
        </div>
        <div class="clearboth"></div>


        <div id="finder" class="dash innerdiv">
            <p class="title">Finders</p>
            <ul>
                <li><?php echo anchor('#', 'host', array('class' => 'host showqtip','title'=>lang('tooltip_host_finder'), 'id' => 'findhost')); ?></li>
                <li><?php echo anchor('widget/allclasses', 'class', array('class' => 'classes showqtip','title'=>lang('tooltip_class_finder'), 'id' => 'findclass')); ?></li>
                <li><?php echo anchor('widget/allpolicies', 'promises', array('class' => 'policies showqtip','title'=>lang('tooltip_promise_finder'), 'id' => 'findpolicy')); ?></li>
                <li><?php echo anchor('widget/allreports', 'reports', array('class' => 'report showqtip', 'id' => 'findreport','title'=>lang('tooltip_reports_finder'))); ?></li>
                <li><?php echo anchor('summaryreports/search', 'summary reports', array('class' => 'summary_report showqtip','id' => 'summaryreport','title'=>lang('tooltip_summary_finder'))); ?></li>
                
                
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
        
        
        //fix for manintaining the layout of the page when window is resized.
        $('#compliance_summary').css('width','50px');
        
        
        
        
        
    });
</script>