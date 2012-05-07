<?php if (isset($saved_search_menu) && !empty($saved_search_menu))
{ ?>
    <?php
    foreach ((array) $saved_search_menu as $cat => $searches)
    {
        ?>
        <h3 class="head"><em>&nbsp;</em><?php echo $cat; ?></h3>
        <div>
            <?php foreach ($searches as $searchObj)
            { ?>
                <a href="<?php echo $searchObj->getUrl(); ?>" class="savesearch-url"><?php echo$searchObj->getLabel(); ?></a>
            <?php } ?>
        </div>
    <?php }
} else { ?>
        <div>No save searches found.</div>
<?php } ?>