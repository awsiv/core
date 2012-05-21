<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>

<!-- <div id="body" class="outerdiv grid_12"> -->
            <div id="topn" class="tables">
                
                <?php if($report_result['meta']['count']> 0){ ?>
                <table>
                    <thead></thead>
                    <?php foreach ((array)$report_result['data'] as $index=>$val) {
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
                                <a href="<?php echo site_url("/knowledge/knowledgeSearch/topic/goals"); ?>" target="_blank">Goals</a>
                            </td>
                            <td>
                                <a href="<?php 
                                    $tempvar = explode("=", $val['key']);
                                    $hostclass = "PK_SHA_" . $tempvar[1];
                                    echo site_url('/search/index/host/All/report/contexts') .  "/inclist/" . urlencode($hostclass) ?>" target="_blank">Classes</a>
                            </td>
                        </tr>

                    <?php } ?>
                </table>
                <?php 
                    include 'paging_footer.php';
                    ?>
                <?php } else{?>
                    <div class="clearboth"></div><div class='info'><?php echo $this->lang->line("no_data")  ?></div>
                
                 <?php }?>
            </div>
<!-- </div> -->
<div class="clear"></div>
<script type="text/javascript">
    // generate the graph script

<?php foreach ($report_result['data'] as $index => $val) { ?>
        var json<?php echo $index ?> = {
           color: ['#779a62','#f0d76a','#cc4358'],
            'label': <?php echo $val['graphSeries']['labels']; ?>,
            'values':  <?php echo $val['graphSeries']['values']; ?>
        };
        $('#<?php echo $index ?>').hostMeter({data:json<?php echo $index ?>});
<?php } ?>
 $(document).ready(function() {
    $('#topn').find('table').tablesorter({widgets: ['zebra']});
 });
</script>