<?php $displayText = ($report_type == "promise-compliance") ? 'promise' : 'bundle'; ?>
<?php
if (is_array($report_result) && !empty($report_result['meta'])) {
    if (isset($report_result['meta']['old_skipped']) &&
        $report_result['meta']['old_skipped'] > 0) {
        echo "<div class='warning'> Warning: You are running an older version of CFEngine on ";
        echo $report_result['meta']['old_skipped'];
        echo " ";
        if ($report_result['meta']['old_skipped'] == 1) {
            echo "host";
        } else {
            echo "hosts";
        }
        echo " within selected context. Data from these clients is not compatible with this report and has therefore been excluded.</div>";
    }
}
?>
<?php if (isset($show_host_summary) && ($show_host_summary == true)){ ?>
            <div id="hostgrpsummary">
                <div class="grid_6" style="margin:0 1%">
                    <div class="hostsCompliance">
                        <p class="title">Red, Yellow and Green compliance for specified <?php echo $displayText; ?>(s).</p>
                        <div class="colourEntry"> 
                            <span class="colourEntryIcon red"></span>
                            <a href="<?php echo site_url( "/search/index/" . $detail_result_url . "/hosts_only/true/clevel/red/"); ?>" title="" class="hostlink"><?php echo $c_data['red']; ?> <?php echo $this->lang->line('not_compliant'); ?></a> 
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/red"); ?>" title="" class="reportlink" >&lt; Show detailed report &gt;</a>
                        </div>
                        <div class="colourEntry">
                            <span class="colourEntryIcon yellow"></span>
                            <a href="<?php echo site_url("search/index/" . $detail_result_url. "/hosts_only/true/clevel/yellow/"); ?>" title=""  class="hostlink"><?php echo $c_data['yellow'] ?> <?php echo $this->lang->line('now_compliant'); ?></a>
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/yellow"); ?>" title="" class="reportlink">&lt; Show detailed report &gt;</a>
                        </div>
                        <div class="colourEntry">
                            <span class="colourEntryIcon green"></span>
                            <a href="<?php echo site_url("search/index/".  $detail_result_url .  "/hosts_only/true/clevel/green/"); ?>" title="" class="hostlink"><?php echo $c_data['green'] ?> <?php echo $this->lang->line('compliant'); ?> </a> 
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/green"); ?>" title="" class="reportlink">&lt; Show detailed report &gt;</a>
                        </div>
                    </div>
                </div>
                <div class="grid_6" style="margin:0 1%">
                    <div class="hostsConnectivity">
                        <p class="title">Blue and Black hosts.</p>
                        <div class="colourEntry">
                            <span class="colourEntryIcon blue"></span>
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url. "/hosts_only/true/clevel/blue/"); ?>" title=""  class="hostlink"><?php echo $c_data['blue_hosts'] ?> <?php echo $this->lang->line('unreachable'); ?> </a> 
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/blue"); ?>" title="" class="reportlink">&lt; Show detailed report &gt;</a>
                        </div>
                        <div class="colourEntry">
                             <span class="colourEntryIcon black"></span>
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/hosts_only/true/clevel/black/"); ?>" title="" class="hostlink"><?php echo $c_data['black_hosts']; ?> <?php echo $this->lang->line('not_executing'); ?></a> 
                            <a href="<?php echo site_url("/search/index/" . $detail_result_url . "/clevel/black"); ?>" title="" class="reportlink">&lt; Show detailed report &gt;</a>
                        </div>
                    </div>
                </div>
                <div class="clear"></div>
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
                    <td><?php 
                    // convert host name and add it as a search criteria to set context in contexfinder
                    $tempvar = explode("=", $rows['hostkey']);
                    $PK_HOST = "inclist/PK_SHA_" . $tempvar[1];
                    echo anchor('search/index/' . $detail_result_url . "/".$PK_HOST, $rows['hostname'], ""); ?></td>
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
