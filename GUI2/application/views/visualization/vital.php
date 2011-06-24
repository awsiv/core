<div id="body">
    <div class="outerdiv">
        <div class="innerdiv">
            <div id="tabpane">
                <div class="pagepanel">
                    <div class="panelhead">Vital signals</div>
                    <div class="panelcontent">
                        <div class="vitaltable">
                            <div id="graph-pulses-vital-sign">
                                <?php foreach ($performanceData['obs'] as $index => $fetchData) {
                               // foreach ($performanceData as $name){
                                    ?>
                                    <h3><a href="#"><?php echo $fetchData['id']; ?> (<?php echo $fetchData['desc']; ?>)<br/>
                                            <?php echo date('D F d h:m:s Y', $performanceData['ls']); ?></a>
                                    </h3>
                                    <div>

                                        <div class="tabs">
                                            <ul>
                                                <li><a href="/graph/magnifiedView/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>"><span>Past 4 hours</span></a></li>
                                                <li><a href="/graph/weekview/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>"><span>Past Week</span></a></li>
                                                <li><a href="/graph/yearview/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>"><span>Year View</span></a></li>
                                                <li><a href="/graph/histogramView/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>"><span>Statistical complete history</span></a></li>
                                            </ul>
                                        </div>
                                    </div>
                                <?php } ?>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>


<script type="text/javascript">

    // tabs
    $tabs = $( ".tabs" ).tabs({
        selected: null,
        cache: true,
        ajaxOptions: {
            error: function( xhr, status, index, anchor ) {
                $( anchor.hash ).html(
                "Couldn't load this tab. We'll try to fix this as soon as possible. " );
            }
        },
        spinner: 'Retrieving data...'
    });



    $("#graph-pulses-vital-sign").addClass("ui-accordion ui-accordion-icons ui-widget ui-helper-reset")
    .find("h3")
    .addClass("ui-accordion-header ui-helper-reset ui-state-default ui-corner-top ui-corner-bottom")
    .hover(function() { $(this).toggleClass("ui-state-hover"); })
    .prepend('<span class="ui-icon ui-icon-triangle-1-e"></span>')
    .click(function() {
        $(this)
        .toggleClass("ui-accordion-header-active ui-state-active ui-state-default ui-corner-bottom")
        .find("> .ui-icon").toggleClass("ui-icon-triangle-1-e ui-icon-triangle-1-s").end()
        .next().slideToggle().toggleClass("ui-accordion-content-active");
        var $link = $('.ui-tabs-nav li:eq(1) a',$(this).next());       
        $link.trigger('click');
        return false;
    })
    .next()
    .addClass("ui-accordion-content  ui-helper-reset ui-widget-content ui-corner-bottom")
    .hide();

    $("#accordion")
    .find("h3 a")
    .focus(function() { $(this).parent().toggleClass("ui-state-hover"); })
    .focusout(function() { $(this).parent().toggleClass("ui-state-hover"); })


   

   


</script>