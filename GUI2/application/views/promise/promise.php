<div id="body">
    <div class="outerdiv">
        <?php if(isset($error)){ ?>
        <div class="innerdiv">
           <?php echo $error ?>
        </div>
       <?php }else{ ?>
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a target="_self" href="#tabs-1"><?php echo $this->lang->line('promise_tab_definition'); ?></a></li>
                <li><a href="#tabs-2" target="_self"><?php echo $this->lang->line('promise_tab_leads'); ?></a></li>
                <li><a href="#tabs-3" target="_self"><?php echo $this->lang->line('promise_tab_same_bundle'); ?></a></li>
                <li><a href="#tabs-4" target="_self"><?php echo $this->lang->line('promise_tab_same_promiser'); ?></a></li>
                <li><a href="#tabs-5" target="_self"><?php echo $this->lang->line('promise_tab_same_type'); ?></a></li>

            </ul>
            <div id="tabs-1" class="ui-corner-all">

                <div class="panelcontent">
                    <table class="bundlelist-table">
                        <tr>
                            <th scope="col" colspan="2"><?php echo $this->lang->line('header_promise_definition'); ?></th>
                        </tr>
                        <tr>
                            <td><?php echo $this->lang->line('promise_belong_to_bundle'); ?></td>
                            <td><a href="<?php echo site_url(); ?>/bundle/details/bundle/<?php echo urlencode($promise['bundlename']); ?>/type/<?php echo urlencode($promise['bundletype']); ?>"><?php echo $promise['bundlename']; ?></a></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_reference_handle'); ?></td>
                            <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['handle']); ?>"><?php echo $promise['handle']; ?></a></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_affected_object'); ?></td>
                            <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promiser']); ?>"><?php echo $promise['promiser']; ?></a></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_stakeholder'); ?></td>
                            <td><?php echo $promise['promisee']; ?></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_comment_intention'); ?></td>
                            <td><?php echo $promise['comment']; ?></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_about'); ?></td>
                            <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['promise_type']); ?>"><?php echo $promise['promise_type']; ?></a></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_class_context'); ?></td>
                            <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo urlencode($promise['class_context']); ?>"><?php echo $promise['class_context']; ?></a></td>
                        </tr>

                        <tr>
                            <td><?php echo $this->lang->line('promise_defined_file'); ?></td>
                            <td><?php echo $promise['file']; ?></td>
                        </tr>

                        <tr >
                            <th scope="blue" colspan="2"><?php echo $this->lang->line('header_body_promise'); ?></th>
                        </tr>

                        <?php 
                        if($promise && key_exists('body', $promise)){
                        foreach ((array) $promise['body'] as $body) { ?>
                            <tr>
                                <td><a href="<?php echo site_url(); ?>/knowledge/knowledgeSearch/topic/<?php echo urlencode($body['type']); ?>"><?php echo $body['type'] ?></a> =></td>
                               <?php if(is_execptional_body(urlencode($body['type']))) {?>
                                <td><a href="<?php echo site_url(); ?>/bundle/details/bundle/<?php echo urlencode($body['name']) ?>"><?php echo $body['name'] ?></a></td>
                                <?php } else{?>
                                <td><a href="<?php echo site_url(); ?>/welcome/body/body/<?php echo urlencode($body['name']); ?>/type/<?php echo urlencode($body['type']) ?>"><?php echo $body['name'] ?><?php echo $body['args'] ?></a></td>
                                 <?php } ?>
                            </tr>
                        <?php } }else{
                            echo "<tr><td colspan=\"2\">". $this->lang->line('promise_body_not_found')."</td></tr>";
                        }?>
                    </table>                 
                </div>
            </div>
            <div id="tabs-2" class="ui-corner-all">
                <?php
                if (is_array($topicLeads) && !empty($topicLeads)) {
                    $this->load->view('knowledge/leads.php');
                }else{
                ?>
                <span>No topics found</span>
                <?php } ?>
            </div>

            <div id="tabs-3" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col"><?php echo $this->lang->line('promise_other_promise'); ?> <a href="<?php echo site_url(); ?>/bundle/details/bundle/<?php echo urlencode($mybundle); ?>"><span id="bundle"><?php echo $mybundle; ?></span></a></th>
                    </tr>


                    <?php foreach ((array) $allhandles as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='%s/promise/details/%s'>%s</a>", site_url(),$p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table>    

            </div>
            <div id="tabs-4" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col"><?php echo $this->lang->line('promise_other_promiser'); ?></th>
                    </tr>


                    <?php foreach ((array) $allhandlespromiser as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='%s/promise/details/%s'>%s</a>", site_url(),$p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table>  
            </div>
            <div id="tabs-5" class="ui-corner-all">

                <table class="bundlelist-table">
                    <tr>
                        <th scope="col"><?php echo $this->lang->line('promise_other_promiser_type'); ?> <?php echo $type ?></th>
                    </tr>


                    <?php foreach ((array) $allhandlesbytype as $p) { ?>
                        <tr>
                            <td><?php echo sprintf("<a href='%s/promise/details/%s'>%s</a>",  site_url(), $p, $p); ?></td> 
                        </tr>
                    <?php } ?>   
                </table> 
            </div>
        </div>
    <?php } ?>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    jQuery(document).ready(function(){
        $("#custom-tabs" ).tabs();
    });
</script>











