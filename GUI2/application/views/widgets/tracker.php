<div style="width:500px;min-height: 300px;overflow: auto;">

    <div id="custom-tabs">
        <ul>
            <?php foreach ($envList as $env) { ?> 
                <li><a  target="_self" href="#<?php echo $env; ?>"><?php echo $env ?></a></li>
            <?php } ?>
        </ul>
        <?php foreach ($envList as $e) { ?> 
            <div id="<?php echo $e; ?>" class="ui-corner-all">
                <!-- generate the table -->
                <table class="bundlelist-table">
                    <tr>
                        <th scope="red">Promise not kept</th>
                    </tr>
                    <?php if (is_array($notkept[$e]['data']) && !empty($notkept[$e]['data'])) {
                        foreach ((array) $notkept[$e]['data'] as $p) { ?>
                            <tr>
                                <td><?php echo ($p[1]); ?></td>
                            </tr>                 
        <?php }    } else { ?>                 
                        <tr>
                            <td>No data found</td>
                        </tr>                 
    <?php } ?>
                </table>

            </div>
<?php } ?>
    </div>
</div>
<script id="source" language="javascript" type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>