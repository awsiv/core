<div id="body" class="grid_12">
    <div class="outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1">Bundle Details</a></li>
                <li><a href="#tabs-2">Bundles using this bundle</a></li>
                <li><a href="#tabs-3">All Bundles</a></li>
            </ul>
            <div id="tabs-1" class="ui-corner-all">
                <div class="panel">

                    <div class="panelcontent">

                        <p class="title">Bundle definition of <?php echo $bundle ?></p>

                        <p><i>Name:</i> <span id="bundle"><?php echo $bundle ?></span></p>
                        <p><i>Arguments:</i> <span id="args"><?php echo $args ?></span></p>

                        <div>

                            <table class="bundlelist-table">
                                <tr>
                                    <th scope="col">HANDLE</th>
                                    <th scope="col">COMMENT</th>
                                </tr>
                                <tr>
                                    <th scope="host-class">Host classes using this bundle </td>
                                    <th scope="host-class"></td>
                                </tr>

                                <?php foreach ((array) $classes as $c) {
                                    ?>
                                    <tr>
                                        <td><?php echo sprintf("<a href='/knowledge/knowledgemap/pid/%s'>%s</a>", $c[0], $c[1]); ?></td>
                                        <td></td>
                                    </tr>
                                <?php } ?>








                                <tr>
                                    <th scope="promise">Promises in this bundle </td>
                                    <th scope="promise"></td>
                                </tr>

                                <?php foreach ((array) $list as $promise) {
                                    ?>
                                    <tr>
                                        <td><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $promise, $promise); ?></td>
                                        <td></td>
                                    </tr>
                                <?php } ?>


                            </table>






                        </div>

                    </div>
                </div>
            </div>

            <div id="tabs-2" class="ui-corner-all">      
                <table class="bundlelist-table">
                    <tr>
                        <th scope="col">Bundles</th>
                    </tr>

                    <?php foreach ((array) $others as $o) {
                        ?>
                        <tr>
                            <td><?php echo sprintf("<a href='/bundle/details/bundle/%s/type/%s'>%s</a>", $o['bundlename'], $o['bundletype'], $o['bundlename']); ?></td>
                        <?php } ?>
                    </tr>
                </table>
            </div>

            <div id="tabs-3" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col">ALL BUNDLE</th>
                    </tr>


                    <?php foreach ((array) $allbundles as $bundles) { ?>
                        <tr>       
                            <td><?php echo sprintf("<a href='/bundle/details/bundle/%s/type/%s'>%s</a>", $bundles[1], $bundles[0], $bundles[1]); ?></td>
                        </tr>
                    <?php } ?>
                </table>     

            </div>

        </div>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>