<?php
#
# This file is (C) Cfengine AS. All rights reserved
?>
<div id="tabpane">
    <div class="pagepanel">
        <div class="panelhead">Vital signals </div>
        <div class="panelcontent">
            <div class="vitaltable">
                <div id="graph-pulses-vital-sign">
                    <?php foreach ($performanceData['obs'] as $index => $fetchData) {
                    ?>

                        <h3><a href="#"><?php echo $fetchData['id']; ?>  <?php echo $performanceData['ls']; ?> </a></h3>
                        <div>

                            <div class="tabs">
                                <ul>
                                    <li><a href="/graph/magnifiedView/obs/<?php echo $fetchData['i']; ?>/host/<?php echo $hostKey; ?>">Past 4 hours</a></li>
                                    <li><a href="/graph/weekview/obs/<?php echo $fetchData['i']; ?>/host/<?php echo $hostKey; ?>">Past Week</a></li>
                                    <li><a href="/graph/histogramView/obs/<?php echo $fetchData['i']; ?>/host/<?php echo $hostKey; ?>">Statistical complete history</a></li>
                                </ul>
                            </div>
                        </div>
                    <?php } ?>
                </div>
            </div>
        </div>
    </div>
</div>



<script type="text/javascript">

     // tabs
    $tabs = $( ".tabs" ).tabs({
        selected: null,
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
        .next().toggleClass("ui-accordion-content-active").slideToggle();
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