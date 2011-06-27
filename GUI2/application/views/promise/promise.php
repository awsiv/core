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

                <div class="panelcontent">
                    <table class="bundlelist-table">
                        <tr>
                            <th scope="col" colspan="2">Promise definition</th>
                        </tr>
                        <tr>
                            <td>Belonging to agent bundle</td>
                            <td><a href="/bundle/details/bundle/<?php echo urlencode($promise['bundlename']); ?>/type/<?php echo urlencode($promise['bundletype']); ?>"><?php echo $promise['bundlename']; ?></a></td>
                        </tr>

                        <tr>
                            <td>Reference handle</td>
                            <td><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['handle']); ?>"><?php echo $promise['handle']; ?></a></td>
                        </tr>

                        <tr>
                            <td>Affected object (promiser)</td>
                            <td><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promiser']); ?>"><?php echo $promise['promiser']; ?></a></td>
                        </tr>

                        <tr>
                            <td>Stakeholders (promisees)</td>
                            <td><?php echo $promise['promisee']; ?></td>
                        </tr>

                        <tr>
                            <td>Comment on original intention</td>
                            <td><?php echo $promise['comment']; ?></td>
                        </tr>

                        <tr>
                            <td>Promise is about</td>
                            <td><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promise_type']); ?>"><?php echo $promise['promise_type']; ?></a></td>
                        </tr>

                        <tr>
                            <td>Applies in the class context</td>
                            <td><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['class_context']); ?>"><?php echo $promise['class_context']; ?></a></td>
                        </tr>

                        <tr>
                            <td>Defined in file</td>
                            <td><?php echo $promise['file']; ?></td>
                        </tr>

                        <tr >
                            <th scope="blue" colspan="2">Body of the promise</th>
                        </tr>

                        <?php foreach ((array) $promise['body'] as $body) { ?>
                            <tr>
                                <td><a href="/knowledge/knowledgeSearch/topic/<?php echo urlencode($body['type']); ?>"><?php echo $body['type'] ?></a> =></td><td><a href="/welcome/body/body/<?php echo urlencode($body['name']); ?>/type/<?php echo urlencode($body['type']) ?>"><?php echo $body['name'] ?><?php echo $body['args'] ?></a></td>
                            </tr>
                        <?php } ?>
                    </table>                 
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

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col">Other promises in bundle <a href="/bundle/details/bundle/<?php echo urlencode($mybundle); ?>"><span id="bundle"><?php echo $mybundle; ?></span></a></th>
                    </tr>


                    <?php foreach ((array) $allhandles as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table>    

            </div>
            <div id="tabs-4" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col">Other promises by promiser</th>
                    </tr>


                    <?php foreach ((array) $allhandlespromiser as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table>  
            </div>
            <div id="tabs-5" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col">Other promises of type <?php echo $type ?></th>
                    </tr>


                    <?php foreach ((array) $allhandlesbytype as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='/promise/details/%s'>%s</a>", $p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table> 
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











