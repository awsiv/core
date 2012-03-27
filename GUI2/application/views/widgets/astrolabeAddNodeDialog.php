<div class="astrolabe addNodeDialog">
    <div>
        <div class="leftColumn">
            <label for="astrolabe-add-node-label">Label</label>
        </div>
        <div class="rightColumn">
            <input id="astrolabe-add-node-label" class="required" minlength="1" value="<?php echo $label ?>"/>
            <p id="astrolabe-add-node-label-error" class="astrolabeAddNodeDialogError"></p>
        </div>
    </div>
    <div class="clear"></div>
    <div>
        <div class="leftColumn">
            <label for="astrolabe-add-node-class">Class Expression</label>
        </div>
        <div class="rightColumn">
            <div class="classInputContainer">
                <input id="astrolabe-add-node-class" minlength="1" value="<?php echo $class ?>"/>
                <span class="classFinderIcon">&nbsp;</span>
            </div>
            <p id="astrolabe-add-node-class-error" class="astrolabeAddNodeDialogError"></p>
        </div>
    </div>
</div>

<script type="text/javascript">

    $('.astrolabe.addNodeDialog .classFinderIcon').classfinder({
        defaultbehaviour: false,
        baseUrl: '<?php echo site_url() ?>',

        complete: function(event,data)
        {
            var $classInput = $('#astrolabe-add-node-class');
            var value = $.trim($classInput.val());
            if (value) {
                value = value + '|';
            }
            $classInput.val(value + data.selectedclass);
        }
    });

</script>