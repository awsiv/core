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
                    <?php foreach ($ret['data'] as $index=>$val) {
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
                <?php $pg = paging($current, $number_of_rows, $ret['meta']['count'], 10);?>
                <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?= site_url('welcome/weakest_host/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?= site_url('welcome/weakest_host/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $current = 'current'; else
                                $current="";
                        ?>

                            <a href="<?= site_url("welcome/weakest_host/rows/$number_of_rows/page/$i") ?>" title="Go to Page <?= $i ?>" class="page <?= $current ?>"><span><?= $i ?></span></a>

                        <? } ?>

                        <a href="<?= site_url('welcome/weakest_host/page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?= site_url('welcome/weakest_host/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>
                <div>
                    <?php
                    echo form_open('welcome/weakest_host/');
                    echo form_input('rows', $number_of_rows);
                    echo "Rows/Page";
                    echo form_close();
                    ?>
                </div>
            </div>
            </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    // generate the graph script

<?php foreach ($ret['data'] as $index => $val) { ?>
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