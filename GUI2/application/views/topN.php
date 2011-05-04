<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>

<div id="body" class="outerdiv grid_12">
            <div id="topn" class="innerdiv tables">
                <p class="title">Weakest Hosts</p>
                <table>
                    <thead></thead>
                    <?php foreach ($ret as $index => $val) {
                    ?>
                        <tr>
                            <td>
               <a href="<?php echo site_url('welcome/hosts/'.$val['colour'])?>" class="<?php echo($val['colour']."host")?> coloricon"></a>
                        </td>
                            <td><a href="<?php echo site_url("/welcome/host/$val[key]"); ?>"><?php echo $val['id']; ?></a></td>
                            <td>
                                <div id="<?php echo($index); ?>" style="height: 100px;width:300px;margin:5px;">
                                </div>
                            </td>
                            <td>
                               <!-- <table>
                                    <tr>
                                        <td><a href="<?php echo site_url("/bundle/blist/$val[key]"); ?>">Bundles</a></td>
                                        <td><a href="<?php echo site_url("/welcome/classes/$val[key]"); ?>">Classes</a></td>

                                    </tr>
                                    <tr>
                                        <td><a href="<?php echo site_url("/welcome/knowledge/topic/goals"); ?>">Goals</a></td>
                                        <td><a href="<?php echo site_url("/promise/plist/$val[key]"); ?>">Promises</a></td>
                                    </tr>
                                </table>-->
                              <a href="<?php echo site_url("/welcome/knowledge/topic/goals"); ?>">Goals</a>
                            </td>
                            <td>
                               <a href="<?php echo site_url("/welcome/classes/$val[key]"); ?>">Classes</a>
                            </td>

                        </tr>

                    <?php } ?>
                </table>
            </div> 
</div>
<div class="clear"></div>
<script type="text/javascript">
    // generate the graph script
<?php foreach ($ret as $index => $val) { ?>
        var json<?php echo $index ?> = {
            color: ['#A3DF00','#EEEE00','#D43030'],
            'label': <?php echo $val['graphSeries']['labels']; ?>,
            'values':  <?php echo $val['graphSeries']['values']; ?>
        };
        $('#<?php echo $index ?>').hostMeter({data:json<?php echo $index ?>});
<?php } ?>
 $(document).ready(function() {
    $('#topn').find('table').tablesorter({widgets: ['zebra']});
 });
</script>