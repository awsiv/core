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

        <ul id="saved_searches_wrapper">
            <li id="saved-searches"><em class="separater"></em><span>Saved Searches</span>
                <ul><li>
                        <div id="searchaccordian">

                        </div>
                    </li>
                </ul>
            </li>
        </ul>
</div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $('#searchaccordian .head').click(function() {
            $(this).next().toggle();
            $(this).toggleClass('expanded');
            return false;
        }).next().hide();

        $("#saved-searches").bind("mouseenter",function(){
            var menuUrl = '<?php echo site_url(); ?>'+'/search/generateSaveSearchMenu';
            $("#searchaccordian").html('loading ....');
            $("#searchaccordian").load(menuUrl,"", function(responseText, textStatus, XMLHttpRequest) {
                if(textStatus == 'error') {
                    $('#searchaccordian').html('Cannot load saved searches.');
                }
            });
        });
    });
</script>