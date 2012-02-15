<div id="body" class="">
    <div class="outerdiv">
          <?php if(isset($error)){ ?>
        <div class="innerdiv">
          <?php echo $error ?>
        </div>
          <?php }else{ ?>
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1"><?php echo $this->lang->line('bundle_tab_details'); ?></a></li>
                <li><a href="#tabs-2"><?php echo $this->lang->line('bundle_tab_bundle_using'); ?></a></li>
                <li><a href="#tabs-3"><?php echo $this->lang->line('bundle_tab_all_bundle'); ?></a></li>
            </ul>
            <div id="tabs-1" class="ui-corner-all">
                <div class="panel">

                    <div class="panelcontent">

                        <p class="title"><?php echo $this->lang->line('bundle_definition_of'); ?> <?php echo $bundle ?></p>

                        <p><b><?php echo $this->lang->line('bundle_name'); ?>:</b> <span id="bundle"><?php echo $bundle ?></span></p>
                        <p><b><?php echo $this->lang->line('bundle_arguments'); ?>:</b> 
                            <?php echo implode(', ', (array)$args) ?>
                        </p>

                        <div>

                            <table class="bundlelist-table">
                                <tr>
                                    <th scope="col"><?php echo $this->lang->line('bundle_handle'); ?></th>
                                    <th scope="col"><?php echo $this->lang->line('bundle_comment'); ?></th>
                                </tr>
                                <tr>
                                    <th scope="host-class" colspan="2"><?php echo $this->lang->line('bundle_host_class'); ?> </th>
                                </tr>

                                <?php if (is_array($classes) && !empty($classes)) {
                                    foreach ((array) $classes as $c) { ?>
                                        <tr>
                                            <td><?php echo sprintf("<a href='%s/knowledge/knowledgemap/pid/%s'>%s</a>",  site_url(), $c[0], $c[1]); ?></td>
                                            <td></td>
                                        </tr>
                                    <?php }
                                } else { ?>
                                    <tr>
                                        <td>None</td>
                                        <td></td>
                                    </tr>
                                <?php } ?>

                                <tr>
                                    <th scope="promise" colspan="2"><?php echo $this->lang->line('bundle_promise'); ?></th>
                                </tr>

                                <?php if (is_array($list) && !empty($list)) {
                                    foreach ((array) $list as $promise) { ?>
                                        <tr>
                                            <td><?php echo sprintf("<a href='%s/promise/details/%s'>%s</a>",  site_url(), $promise, $promise); ?></td>
                                            <td></td>
                                        </tr>
                                    <?php }
                                } else { ?>
                                    <tr>
                                        <td>None</td>
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
                        <th scope="col"><?php echo $this->lang->line('bundle_bundle'); ?></th>
                    </tr>

                    <?php if (is_array($others) && !empty($others)) {
                        foreach ((array) $others as $o) { ?>
                            <tr>
                                <td><?php echo sprintf("<a href='%s/bundle/details/bundle/%s/type/%s'>%s</a>",  site_url(), $o['bundlename'], $o['bundletype'], $o['bundlename']); ?></td>
                            </tr>
                        <?php }
                    } else { ?>
                        <tr>
                            <td>None</td>                            
                        </tr>
                    <?php } ?>
                </table>
            </div>

            <div id="tabs-3" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col"><?php echo $this->lang->line('bundle_tab_all_bundle'); ?></th>
                    </tr>


                    <?php if (is_array($allbundles) && !empty($allbundles)) {
                        foreach ((array) $allbundles as $bundles) { ?>
                            <tr>       
                                <td><?php echo sprintf("<a href='%s/bundle/details/bundle/%s/type/%s'>%s</a>",  site_url(), $bundles[1], $bundles[0], $bundles[1]); ?></td>
                            </tr>
                        <?php }
                    } else { ?>
                        <tr>
                            <td>None</td>                            
                        </tr>
                    <?php } ?>
                </table>     

            </div>

        </div>
         <?php } ?>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>