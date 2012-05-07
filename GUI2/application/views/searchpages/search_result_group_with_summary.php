<?php $displayText = ($report_type == "promise-compliance") ? 'promise' : 'bundle'; ?>
<?php if (isset($show_host_summary) && ($show_host_summary == true)){ ?>
            <div id="hostsInfoContainer" class="hostsInfoContainer innerdiv grid_12" style="">

                <div id="hostsComplianceContainer" class="hostsComplianceContainer grid_7">
                    <div class="hostsCompliance">
                        <p class="title">Red, Yellow and Green compliance in regard to specified <?php echo $displayText; ?>(s).</p>
                        <div class="colourEntry"> <a href="<?php echo site_url( "/search/index/" . $detail_result_url . "/hosts_only/true/clevel/red/"); ?>" title="" class="redhost showqtip hoststatuslist"><?php echo $c_data['red']; ?> <?php echo $this->lang->line('not_compliant'); ?></a> <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/red"); ?>" title="" >Show detail report</a></div>
                        <div class="colourEntry"><a href="<?php echo site_url("search/index/" . $detail_result_url. "/hosts_only/true/clevel/yellow/"); ?>" title=""  class="yellowhost showqtip hoststatuslist "><?php echo $c_data['yellow'] ?> <?php echo $this->lang->line('now_compliant'); ?></a> <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/yellow"); ?>" title="">Show detail reports</a></div>
                        <div class="colourEntry"><a href="<?php echo site_url("search/index/".  $detail_result_url .  "/hosts_only/true/clevel/green/"); ?>" title="" class="greenhost showqtip hoststatuslist "><?php echo $c_data['green'] ?> <?php echo $this->lang->line('compliant'); ?> </a> <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/green"); ?>" title="" >Show detail reports</a></div>
                    </div>
                </div>
                <div class="hostsConnectivityContainer grid_5">
                    <div id="hostsConnectivity" class="hostsConnectivity">
                        <p class="title">Blue and Black host  in regard to specified  <?php echo $displayText; ?>(s).</p>
                        <div class="colourEntry">
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/hosts_only/true/clevel/black/"); ?>" title="" class="blackhost showqtip hoststatuslist"><?php echo $c_data['black_hosts']; ?> <?php echo $this->lang->line('not_compliant'); ?></a> <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/black"); ?>" title="" >Show detail report</a>
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url. "/hosts_only/true/clevel/blue/"); ?>" title=""  class="bluehost showqtip hoststatuslist "><?php echo $c_data['blue_hosts'] ?> <?php echo $this->lang->line('now_compliant'); ?> </a> <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/blue"); ?>" title="" >Show detail report</a>
                        </div>
                        <div class="clear"></div>
                    </div>
                </div>
            </div>
        <?php } ?>

        <div class="tables hostlist">
            <p class="title">Hosts in regards to specific  <?php echo $displayText; ?>(s) <?php if ($clevel) { ?> (compliance level: <?php echo $clevel ?>) <?php } ?></p>
        <?php
            if (is_array($report_result) && !empty($report_result['data']))
            { ?>

            <table>
            <thead>
                <tr>
                    <th>Hostname</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                <?php  foreach ((array) $report_result['data'] as $rows) { ?>
                <tr>
                    <td><?php echo anchor('search/index/' . $detail_result_url . "/host/".$rows['hostkey'], $rows['hostname'], ""); ?></td>
                    <td class="actioncol"><?php echo anchor('welcome/host/' . $rows['hostkey'],' ', array('title' => 'Host information', 'class' => 'hostbtn showqtip')).anchor('search/index/report/promises-not-kept-summary/host/' . $rows['hostkey'], ' ', array('title' => 'promises not kept', 'class' => 'promisesnotkeptbtn showqtip'))
                 . anchor('visual/vital/' . $rows['hostkey'], ' ', array('title' => 'pulse and vitals', 'class' => 'vitalsbtn showqtip')); ?> </td>
                </tr>
                <?php } ?>
            </tbody>


        </table>




            <?php    include 'paging_footer.php';
            }
            else
            {
                echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
            }
            ?>
        </div>