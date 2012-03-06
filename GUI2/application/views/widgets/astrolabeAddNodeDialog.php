<form class="astrolabe addNodeDialog form">
    <p>
        <label for="astrolabe-add-node-label">Label</label>
        <input id="astrolabe-add-node-label" class="required" minlength="1" maxlength="50" value="<?php echo $label ?>"/>
    </p>

    <p>
        <label for="astrolabe-add-node-class">Class Expression</label>
        <input id="astrolabe-add-node-class" minlength="1" maxlength="50" value="<?php echo $class ?>"/>
    </p>
</form>