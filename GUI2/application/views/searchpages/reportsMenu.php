<div class="dd_menu">
    <ul>
        <?php foreach ($reports_menu as $key => $val)
        { ?>
            <li><span class="<?php echo $report_category == $key ? "selected" : "" ?>"> <?php echo $key ?></span>
                <ul>
                    <?php foreach ($val as $reportdata)
                    { ?>
                        <li><a id='<?php echo $reportdata['id']; ?>' href="<?php echo site_url('/search/index/host/All/report') . '/' . urlencode($reportdata['id']) . (!empty($incList) ? "/inclist/" . urlencode($incList) : '') . (!empty($exList) ? "/exlist/" . urlencode($exList) : '') ?>" class="<?php echo $reportdata['id'] == $report_type ? "selected" : "" ?>"><?php echo $reportdata['name'] ?></a></li>
                    <?php } ?>
                </ul>
            </li>
        <?php } ?>
        <?php if (isset($saved_search_menu))
        { ?>

            <li><span>Saved Searches</span>
                <ul>
                    <?php
                    foreach ((array) $saved_search_menu as $cat => $searches)
                    {
                        ?>
                        <h1><?php echo $cat; ?></h1>
                        <?php foreach ($searches as $searchObj)
                        { ?>
                            <p><a href="<?php echo $searchObj->getUrl(); ?>" class=""><?php echo$searchObj->getLabel(); ?></a></p>
                        <?php } ?>


            <?php } ?>
                </ul>
            </li>
<?php } ?>



    </ul>
</div>