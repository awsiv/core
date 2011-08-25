<div class="outerdiv grid_12">
    <div class="innerdiv">
        <div class="dash push_1">
            <p class="title">User settings and preferences</p>
            <ul>
                <li><?php echo anchor('/savedsearch/listSavedSearches/'," List of saved searches",array('class' => 'savedsearch'))?></li>
                <?php if($is_displayable){?><li><?php echo anchor('/settings/manage/'," Mission portal settings",array('class' => 'portalsettings')) ?></li><?php }?>
                <li><?php echo anchor('/settings/preferences/'," My preferences",array('class' => 'preference')) ?></li>
                <p class="clearleft"></p>
            </ul>
        </div>
    </div>  
</div>
