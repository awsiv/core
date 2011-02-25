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
                   No host selected . Please select a host to see its vital signs.
                </div>
            </div>
        </div>
    </div>
</div>



<script type="text/javascript">

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
        return false;
    })
    .next()
    .addClass("ui-accordion-content  ui-helper-reset ui-widget-content ui-corner-bottom")
    .hide();

    $("#accordion")
    .find("h3 a")
    .focus(function() { $(this).parent().toggleClass("ui-state-hover"); })
    .focusout(function() { $(this).parent().toggleClass("ui-state-hover"); })


    // tabs
    $( ".tabs" ).tabs({
        selected: null,
        ajaxOptions: {
            error: function( xhr, status, index, anchor ) {
                $( anchor.hash ).html(
                "Couldn't load this tab. We'll try to fix this as soon as possible. " +
                    "If this wouldn't be a demo." );
            }
        }
    });


</script>