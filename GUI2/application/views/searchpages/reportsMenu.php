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
        

    </ul>
    <?php if (isset($saved_search_menu) && !empty ($saved_search_menu)) { ?>    
    <ul id="saved_searches_wrapper">
            <li id="saved-searches"><em class="separater"></em><span>Saved Searches</span>
                <ul><li>
                        <div id="searchaccordian">
                        
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


                        <?php } ?>
                       </div>
                    </li>
                </ul>
            </li>
     </ul>
       <?php } ?>    
</div>

<script type="text/javascript">
	jQuery(document).ready(function(){
	$('#searchaccordian .head').click(function() {
                var self=this;
		$(this).next().toggle();
                $(this).toggleClass('expanded');
		return false;
	}).next().hide();
});
</script>
