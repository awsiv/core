<div class="dd_menu">
    <ul>
        <?php foreach ($reports_menu as $key => $val)
        { ?>
            <li class="<?php echo $report_category == $key ? "selected" : "" ?>"><?php echo $key ?>
                <ul>
                    <?php foreach ($val as $reportdata)
                    { ?>
                        <li><a id='<?php echo  $reportdata['id']; ?>' href="<?php echo site_url('/search/index/host/All/report') . '/' . urlencode($reportdata['id']) ?>" class="<?php echo $reportdata['id'] == $report_type ? "selected" : "" ?>"><?php echo $reportdata['name'] ?></a></li>
            <?php } ?>
                </ul>
            </li>
<?php } ?>
    </ul>
</div>