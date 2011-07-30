<div id="body" class="grid_12">
    <div class="outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1">Body Definition</a></li>
                <li><a href="#tabs-2">Other Bodies</a></li>
            </ul>
            <div id="tabs-1" class="ui-corner-all">

                <div class="panel">
                    <div class="panelcontent">

                        <table class="bundlelist-table">
                            <tr>
                                <th scope="col">Body Definition</th>
                                <th scope="col"></th>
                            </tr>
                            <tr>
                                <td>Type</td>
                                <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo $def['Type']; ?>"><?php echo isset($def['Type'])?$def['Type']:"not found"; ?></a></td>
                            </tr>
                            <tr>
                                <td>Name</td>
                                <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo $def['Name']; ?>"> <?php echo isset($def['Name'])?$def['Name']:"not found"; ?></a></td>
                            </tr>
                            <tr>
                                <td>Arguments</td>
                                <td><?php echo isset($def['Arguments'])?$def['Arguments']:"not found any"; ?></td>
                            </tr>




                            <?php foreach ((array) $def['attributes'] as $att) { ?>
                                <tr>
                                    <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo $att['lval']; ?>"><?php echo $att['lval']; ?></a> => </td><td><?php echo $att['rval']; ?> (<a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo $att['class_context']; ?>"><?php echo $att['class_context']; ?></a>)</td>
                                </tr>
                            <?php } ?>
                        </table>    

                    </div>
                </div>
            </div>

            <div id="tabs-2" class="ui-corner-all">

                <div class="panel"><div class="panelhead">Other bodies of type <?php echo $type ?></div>
                    <div class="panelcontent">
                        <ul style="margin-left: 15px;">
                            <?php foreach ((array) $allbodies as $b) { 
                               if(is_execptional_body($b['type'])) {
                                ?>
                              <li><?php echo sprintf("<a href='%s/bundle/detail/bundle/%s</a>", site_url(), $b['type']); ?> </li>
                            <?php } else{?>
                            <li><?php echo sprintf("<a href='%s/welcome/body/body/%s/type/%s'>%s %s</a>", site_url(),$b['body'], $b['type'], $b['body'], $b['type']); ?></li>
                            <?php }} ?>
                        </ul>
                    </div>
                </div>
            </div>
        </div>
    </div>

</div>    

<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>