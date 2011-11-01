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
                        <?php foreach ($bundle_data['meta']['header'] as $key=>$index) { ?>
                        <tr>
                            <td><?php echo $key; ?></td>
                            <td><?php if ('Created at' != $key) echo $bundle_data['data']['0'][$index]; else echo  getDateStatus($bundle_data['data']['0'][$index]); ?></td>
                        </tr>
                        <?php } ?>
                        
                         <tr>
                            <th scope="col" colspan="2">Status</th>
                        </tr>
                        <?php foreach ($bundle_status['meta']['header'] as $key=>$index) { ?>
                        <tr>
                            <td><?php echo $key; ?></td>
                            <td><?php if ('Created at' != $key) echo $bundle_status['data']['0'][$index]; else echo  getDateStatus($bundle_status['data']['0'][$index]); ?></td>
                        </tr>
                        <?php } ?>

                    </table>                 
                </div>
            </div>
            <div id="tabs-2" class="ui-corner-all">
               
                
                
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











