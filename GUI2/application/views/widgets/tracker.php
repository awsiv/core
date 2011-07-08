<div style="min-width:500px;min-height: 300px;overflow: auto;">

    <div id="custom-tabs">
        <ul>
             <li><a  target="_self" href="<?php echo site_url(); ?>/widget/summaryCompliance/stop/-1/env/">Default</a></li>
            
            <?php foreach ($envList as $env) { ?> 
                <li><a  target="_self" href="<?php echo site_url(); ?>/widget/summaryCompliance/stop/-1/env/environment_<?php echo $env; ?>"><?php echo $env ?></a></li>
            <?php } ?>
        </ul>
    </div>
</div>
<script id="source" language="javascript" type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>