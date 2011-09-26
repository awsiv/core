<div>
    <h1><b><?php echo $this->lang->line('knowledge_insight_leads'); ?>:</b></h1>
    <div style="margin-left: 12px;">
        <ul>
            <?php foreach ((array) $topicLeads as $lead) {
                ?>
            <li style="list-style: none;"><span style="color: #212121;"><?php echo $topicDetail['topic']; ?> ( in <?php echo $topicDetail['context']; ?> ) "<?php echo $lead['assoc']; ?>"</span></li>
                <ul style="padding: 15px;">
                    <?php foreach ((array) $lead['topics'] as $l) { ?>
                        <li>
                            <?php
                            $split = explode('::', $l['topic']);
                            if (is_array($split) && !empty($split) && $split[0] != 'any') {
                                $output = sprintf("<a href='%s/knowledge/knowledgemap/pid/%s'>%s </a> ( in %s )", site_url(),$l['id'], $split[1], $split[0]);
                            } else if (is_array($split) && !empty($split) && $split[0] == 'any') {
                                $output = sprintf("<a href='%s/knowledge/knowledgemap/pid/%s'>%s </a>",  site_url(), $l['id'], $split['1']);
                            }
                            echo $output;
                            ?>
                        </li>
                    <?php } ?>
                </ul>
            <?php } ?>
        </ul>
    </div>
</div>