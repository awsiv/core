<div id="tabs">
    <ul>
        <li><a href="#tabs-1">Bundle Details</a></li>
        <li><a href="#tabs-2">All Bundles</a></li>
    </ul>
    <div id="tabs-1">
        <div class="panel">
            <div class="panelhead">Bundle definition of <?php echo $bundle ?>:</div>
            <div class="panelcontent">
                <div>
                    <ul>
                        <li><i>Name:</i> <span id="bundle"><?php echo $bundle ?></span><br></li>
                        <li><i>Arguments:</i> <span id="args"><?php echo $args ?></span></li>
                        <li><i>Host classes using this bundle:</i> <?php echo $classes ?></li>
                        <li><i>Promises in this bundle:</i>
                            <ul>
                                <?php foreach ((array) $list as $promise) {
 ?>
                                    <li><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $promise, $promise); ?></li>
<?php } ?>
                            </ul>
                        </li>
                        <li><i>Bundles using this bundle:</i>
                        <ul>
                            
                                <?php foreach ((array) $others as $o) {
 ?>
                                    <li><?php echo sprintf("<a href='/bundle/details/bundle/%s/type/%s'>%s</a>", $o['bundlename'], $o['bundletype'],$o['bundlename']); ?></li>
<?php } ?>
                            </ul>


                        </li>
                    </ul>

                </div>

            </div>
        </div>
    </div>
    <div id="tabs-2">
        <ul>
<?php foreach ((array) $allbundles as $bundles) { ?>
        <li><?php echo sprintf("<a href='/bundle/details/bundle/%s/type/%s'>%s</a>", $bundles[1], $bundles[0],$bundles[1]); ?></li>
<?php } ?>
        </ul>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#tabs" ).tabs();
    });
</script>