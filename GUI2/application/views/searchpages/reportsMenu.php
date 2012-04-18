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

            <li id="saved-searches"><span>Saved Searches</span>
                <ul><li>
                        <?php
                        foreach ((array) $saved_search_menu as $cat => $searches)
                        {
                            ?>
                            <p><?php echo $cat; ?></p>
                            <?php foreach ($searches as $searchObj)
                            { ?>
                                <a href="<?php echo $searchObj->getUrl(); ?>" class="savesearch-url"><?php echo$searchObj->getLabel(); ?></a>
                            <?php } ?>


                        <?php } ?>
                    </li>
                </ul>
            </li>
        <?php } ?>



    </ul>
</div>