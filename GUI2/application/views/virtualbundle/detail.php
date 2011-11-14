<div id="body">
    <div class="outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1">Overview</a></li>
                <li><a href="#tabs-2">Promises</a></li>
            </ul>
            <div id="tabs-1" class="ui-corner-all">

                <div class="panelcontent">
                    <table class="bundlelist-table">
                        <tr>
                            <th scope="col" colspan="2"><?php echo $this->lang->line('header_promise_definition'); ?></th>
                        </tr>
                        <?php foreach ($bundle_data['meta']['header'] as $key => $index) { ?>
                            <tr>
                                <td><?php echo $key; ?></td>
                                <td><?php
                        if ('Created at' != $key)
                            echo $bundle_data['data']['0'][$index]; else
                            echo getDateStatus($bundle_data['data']['0'][$index], true);
                            ?></td>
                            </tr>
                        <?php } ?>

                        <tr>
                            <th scope="col" colspan="2">Status</th>
                        </tr>
                        <?php foreach ($bundle_status['meta']['header'] as $key => $index) { ?>
                            <tr>
                                <td><?php echo $key; ?></td>
                                <td><?php
                        if ('Created at' != $key)
                            echo $bundle_status['data']['0'][$index]; else
                            echo getDateStatus($bundle_status['data']['0'][$index]);
                            ?></td>
                            </tr>
                        <?php } ?>

                    </table>                 
                </div>
            </div>
            <div id="tabs-2" class="ui-corner-all">

                <table class="bundlelist-table">

                    <tr>
                        <?php foreach ($bundle_promises['meta']['header'] as $key => $index) { ?>
                            <th><?php echo $key; ?></th>
                        <?php } ?>
                             <th>Action</th>   
                    </tr>

                    <?php foreach ($bundle_promises['data'] as $key => $data) { ?>
                        <tr>

                            <td><?php echo $data[0]; ?></td>
                            <td><?php echo sprintf('<a href="%s/search/index/report/Compliance+by+promise/host/All/name/%s">%s</a>', site_url(), $data[0], $data[1]); ?></td>

                            <td><?php echo sprintf('<a href="%s/search/index/report/Compliance+by+promise/host/All/name/%s/hosts_only/true">%s</a>', site_url(), $data[0], $data[2]); ?></td>

                            <td><?php echo $data[3]; ?></td>
                            <td><?php echo sprintf('<a href="%s/search/index/name/%s/host/All/report/Promises+not+kept+log">%s</a>', site_url(), $data[0],'warning'); ?></td>

                        </tr>
                    <?php } ?>

                    <tr>
                        <td>Total</td>
                        <td><?php echo $bundle_status['data']['0'][1]; ?></td>
                        <td><?php echo $bundle_status['data']['0'][2]; ?></td>
                        <td><?php echo $bundle_status['data']['0'][3]; ?></td>
                         <td>Add promise</td>
                    </tr>

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











