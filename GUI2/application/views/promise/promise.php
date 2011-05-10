<div id="body" class="grid_12">
    <div class="outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1">Definition</a></li>
                <li><a href="#tabs-2">Leads</a></li>
                <li><a href="#tabs-3">Same Bundle</a></li>
                <li><a href="#tabs-4">Same Promiser</a></li>
                <li><a href="#tabs-5">Same Type</a></li>

            </ul>
            <div id="tabs-1" class="ui-corner-all">
                <div class="panelhead">Promise definition: </div>
                <div class="panelcontent">
                    <ul>
                        <li>Belonging to agent bundle : <a href="/bundle/details/bundle/<?php echo urlencode($promise['bundlename']); ?>/type/<?php echo urlencode($promise['bundletype']); ?>"><?php echo $promise['bundlename']; ?></a></li>
                        <li>Reference handle : <a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['handle']); ?>"><?php echo $promise['handle']; ?></a></li>
                        <li>Affected object (promiser)	:<a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promiser']); ?>"><?php echo $promise['promiser']; ?></a></li>
                        <li>Stakeholders (promisees)	:<?php echo $promise['promisee']; ?></li>
                        <li>Comment on original intention	:<?php echo $promise['comment']; ?></li>
                        <li>Promise is about	:<a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promise_type']); ?>"><?php echo $promise['promise_type']; ?></a></li>
                        <li>Applies in the class context	:<a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['class_context']); ?>"><?php echo $promise['class_context']; ?></a></li>
                        <li>Defined in file	: <?php echo $promise['file']; ?> near line <?php echo $promise['line_num']; ?> </li>
                        <li>Body of the promise	: </li>
                        <?php foreach ((array) $promise['body'] as $body) { ?>
                            <li><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($body['type']); ?>"><?php echo $body['type'] ?></a> => <a href="/welcome/body/body/<?php echo urlencode($body['name']); ?>/type/<?php echo urlencode($body['type']) ?>"><?php echo $body['name'] ?><?php echo $body['args'] ?></a></li>
                        <?php } ?>
                    </ul>
                </div>
            </div>
            <div id="tabs-2" class="ui-corner-all">
                <?php
                if (is_array($topicLeads) && !empty($topicLeads)) {
                    $this->load->view('knowledge/leads.php');
                }
                ?>
            </div>

            <div id="tabs-3" class="ui-corner-all">
                <h4>Other promises in bundle <a href="/bundle/details/bundle/<?php echo urlencode($mybundle); ?>"><span id="bundle"><?php echo $mybundle; ?></span></a></h4>
                <ul>
                    <?php foreach ((array) $allhandles as $p) { ?>
                        <li><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></li> 
                    <?php } ?>    
                </ul>   
            </div>
            <div id="tabs-4" class="ui-corner-all">
                <h4>Other promises by promiser</h4>

                <ul>
                    <?php foreach ((array) $allhandlespromiser as $p) { ?>
                        <li><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></li> 
                    <?php } ?>    
                </ul>   
            </div>
            <div id="tabs-5" class="ui-corner-all">

                <h4>Other promises of type <?php echo $type ?></h4>
                <ul>
                    <?php foreach ((array) $allhandlesbytype as $p) { ?>
                        <li><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></li> 
                    <?php } ?>    
                </ul> 
            </div>
        </div>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $( "#custom-tabs" ).tabs();
    });
</script>











