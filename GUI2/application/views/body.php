<div id="tabs">
    <ul>
        <li><a href="#tabs-1">Body Definition</a></li>
        <li><a href="#tabs-2">Other Bodies</a></li>
    </ul>
    <div id="tabs-1">

        <div class="panel"><div class="panelhead">Body definition:</div>
            <div class="panelcontent">
                <ul>
                    <li>Type : <a href="/knowledge/knowledgeSearch/topic/<?php echo $def['Type']; ?>"><?php echo $def['Type']; ?></a></li>
                    <li>Name : <a href="/knowledge/knowledgeSearch/topic/<?php echo $def['Name']; ?>"> <?php echo $def['Name']; ?></a></li>
                    <li>Arguments :  <?php echo $def['Arguments']; ?></li>
                    <?php foreach ((array) $def['attributes'] as $att) { ?>
                        <li><a href="/knowledge/knowledgeSearch/topic/<?php echo $att['lval']; ?>"><?php echo $att['lval']; ?></a> => <?php echo $att['rval']; ?> (<a href="/knowledge/knowledgeSearch/topic/<?php echo $att['class_context']; ?>"><?php echo $att['class_context']; ?></a>)</li>
                    <?php } ?>
                </ul>
            </div>
        </div>
    </div>

    <div id="tabs-2">

        <div class="panel"><div class="panelhead">Other bodies of type <?php echo $type ?></div>
            <div class="panelcontent">
                <ul>
                    <?php foreach ((array) $allbodies as $b) { ?>
                        <li><?php echo sprintf("<a href='/welcome/body/body/%s/type/%s'>%s %s</a>", $b['body'], $b['type'], $b['body'], $b['type']); ?> </a></li>
                    <?php } ?>                    
                </ul>

            </div>
        </div>
    </div>


</div>    

<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#tabs" ).tabs();
    });
</script>