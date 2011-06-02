<div style="width:650px;height: 450px;overflow: auto;">

        <div id="custom-tabs">
            <ul>
                <?php foreach ($envList as $env) { ?> 
                <li><a  target="_self" href="/widget/summaryCompliance/start/-1/stop/-1/env/environment_<?php echo $env; ?>"><?php echo $env ?></a></li>
                <?php } ?>
            </ul>

        </div>
</div>
<script id="source" language="javascript" type="text/javascript">

    jQuery(document).ready(function(){

        $( "#custom-tabs" ).tabs();
       
     

    });


</script>