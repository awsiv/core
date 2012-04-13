<div class="outerdiv grid_12">
    <div class="innerdiv">
        <div class="push_1 settings">
            <p class="title">User settings and preferences</p>
            <ul class="icons_66">
                <li class="savedsearch">
                    <?php echo anchor('/savedsearch/listSavedSearches/'," ",array('class' => 'ico'))?>
                    <?php echo anchor('/savedsearch/listSavedSearches/',"List of saved searches",array('class' => 'txt'))?></li>
                </li>
                <?php if($is_displayable){?>
                    <li class="portalsettings">
                        <?php echo anchor('/settings/manage/'," ",array('class' => 'ico')) ?>
                        <?php echo anchor('/settings/manage/',"Mission Portal settings", array('class' => 'txt')) ?>
                    </li>
                <?php }?>

                <li class="preference">
                    <?php echo anchor('/settings/preferences/'," ",array('class' => 'ico')) ?>
                    <?php echo anchor('/settings/preferences/',"My preferences",array('class' => 'txt')) ?>
                </li>

                <li class="hub_replication showqtip" title="<?php echo $this->lang->line('hub_replication_status') ?>">
                    <?php echo anchor('/hubstatus/status'," ",array('class' => 'ico')) ?>
                    <?php echo anchor('/hubstatus/status',"Hub replication status",array('class' => 'txt ')) ?>
                </li>
               <!-- <li><a href="<?php echo site_url() ?>/hubstatus/status" title="See the status of the hub replication" class="showqtip"><?php echo $this->lang->line('hub_replication_status'); ?></a></li> -->
                    
                
                <?php if(is_constellation()){?>
                    <li classs="preference">
                        <?php echo anchor('/virtualbundle/manage/'," ",array('class' => 'ico')) ?>
                        <?php echo anchor('/virtualbundle/manage/',"My Virtual Bundles",array('class' => 'txt')) ?>
                    </li>
                <?php } ?>
                
                <p class="clearleft"></p>
            </ul>
        </div>
    </div>  
</div>
