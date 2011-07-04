<div id="body">
    <div class="outerdiv">
        <div id="hoststatus" class="minwidth30 innerdiv">
         <p class="title">Host Status</p>
        <ul>
            <!--<li>Hosts Known: <strong><?php echo $all ?></strong></li>
            <li>Ranked worst: <a href="weakest_host">Show top 50</a></li>-->
            <li> <a href="<?php echo site_url('welcome/hosts/red')?>" class="redhost hoststatuslist"><?php echo $r ?> hosts known</a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/yellow')?>" class="yellowhost hoststatuslist"><?php echo $y ?> hosts </a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/green')?>" class="greenhost hoststatuslist"><?php echo $g ?> hosts known</a> </li>
            <li> <a href="<?php echo site_url('welcome/hosts/blue')?>" class="bluehost hoststatuslist"><?php echo $b ?> hosts known</a> </li>
        </ul>

            <ul>
                <li>Hosts Known: <strong><?php echo $all ?></strong></li>
                <li>Ranked worst: <a href="weakest_host">Show top 50</a></li>
            </ul>
        </div>
        <div id="summarymetereng" class="minwidth70 innerdiv">
            <p class="title">Averages summarizing all host</p>
            <div class="graphcontainer">
                <?php include_once('graph/summaryCompliance.php'); ?>
            </div>
        </div>
        <div class="clearboth"></div>


        <div id="finder" class="dash innerdiv">
            <p class="title">Finders</p>
            <ul>
                <li><?php echo anchor('#', 'host', array('class' => 'host', 'id' => 'findhost')); ?></li>
                <li><?php echo anchor('widget/allclasses', 'class', array('class' => 'classes', 'id' => 'findclass')); ?></li>
                <li><?php echo anchor('widget/allpolicies', 'promises', array('class' => 'policies', 'id' => 'findpolicy')); ?></li>
                <li><?php echo anchor('widget/allreports', 'reports', array('class' => 'report', 'id' => 'findreport')); ?></li>
                <p class="clearleft"></p>
            </ul>

        </div>
    </div>
</div>
<div class="clear"></div>

<script type="text/javascript">
    $(document).ready(function(){
        $('#findhost').hostfinder();
        $('#findclass').classfinder();
        $('#findpolicy').policyfinder();
        $('#findreport').reportfinder();
        //fix for manintaining the layout of the page when window is resized.
        $('#compliance_summary').css('width','50px');
    });
</script>