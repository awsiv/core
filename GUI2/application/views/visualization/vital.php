<div id="body">
    <div class="outerdiv">
        <div class="innerdiv">
            <div id="tabpane">
                <div class="pagepanel">
                    <div class="panelhead">Vital signals of <?php echo sprintf("%s (%s)", $performanceData['hostname'], $performanceData['ip']); ?></div>
                    <div class="panelcontent">
                        <div class="vitaltable">
                            <div id="graph-pulses-vital-sign">
                                <?php
                                krsort($performanceData['obs']);
                                foreach ($performanceData['obs'] as $index => $fetchData)
                                {
                                    ?>
                                    <h3 id="<?php echo $fetchData['id']; ?>"><a href="#"><?php echo $fetchData['id']; ?> <?php echo $fetchData['desc'] ? "(" . $fetchData['desc'] . ")" : ''; ?>  <?php echo ($fetchData['units']) ? "[" . urldecode($fetchData['units']) . "]" : ''; ?><br/>
    <?php echo getDateStatus($performanceData['ls'], true); ?></a>
                                    </h3>
                                    <div>

                                        <div class="tabs">
                                            <ul>
                                                <li><a href="<?php echo site_url(); ?>/graph/magnifiedView/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>/units/<?php echo $fetchData['units']; ?>"><span>Past 4 hours</span></a></li>
                                                <li><a href="<?php echo site_url(); ?>/graph/weekview/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>/units/<?php echo $fetchData['units']; ?>"><span>Past Week</span></a></li>
                                                <li><a href="<?php echo site_url(); ?>/graph/yearview/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>/units/<?php echo $fetchData['units']; ?>"><span>Year View</span></a></li>
                                                <li><a href="<?php echo site_url(); ?>/graph/histogramView/obs/<?php echo $fetchData['id']; ?>/host/<?php echo $hostKey; ?>/units/<?php echo $fetchData['units']; ?>"><span>Statistical complete history</span></a></li>
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
                "Couldn't load this tab." );
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


    // for auto-opening the selected observables
    var auto_open = "<?php echo $obs; ?>";
    if (auto_open) {
        var $autoLink = $("#graph-pulses-vital-sign").find("#"+auto_open);
        $autoLink.trigger('click');
        $('html, body').animate({
            scrollTop: $autoLink.offset().top
        }, 2000);
    }





</script>