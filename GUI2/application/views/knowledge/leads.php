<div style="font-size: 11px;">
    <h3><b><?php echo $this->lang->line('knowledge_insight_leads'); ?> <?php echo $topicDetail['topic']; ?>:</b></h3>
    <div>
        <ul>
            <?php if (is_array($topicLeads) && !empty($topicLeads)) { foreach ((array) $topicLeads as $allLeads) {
                ?>
            <li style="list-style: none;">
                <?php $searchContext =  ($topicDetail['context'] === $allLeads['context'])? true:false; ?>
                <span style="color: #212121;">In the context of <?php echo $allLeads['context'] ?><?php if ($searchContext) { ?> (Searched context)<?php } ?>, <?php echo $topicDetail['topic']; ?></span>
            </li>
            <ul style="padding: 5px;">
                    <?php
                    if (is_array($allLeads['leads']) && !empty ($allLeads['leads'])) {
                    foreach ((array) $allLeads['leads'] as $lead) {
                    foreach ((array) $lead['topics'] as $l) {
                        ?>
                        <li>
                            <?php
                            $split = explode('::', $l['topic']);
                            if (is_array($split) && !empty($split) && $split[0] != 'any') {
                                $output = sprintf("<a href='%s/knowledge/knowledgemap/pid/%s'>%s </a> ( in %s )", site_url(),$l['id'], $split[1], $split[0]);
                            } else if (is_array($split) && !empty($split) && $split[0] == 'any') {
                                $output = sprintf("<a href='%s/knowledge/knowledgemap/pid/%s'>%s </a>",  site_url(), $l['id'], $split['1']);
                            }
                            echo sprintf("%s %s",$lead['assoc'],$output);
                            ?>
                        </li>
                    <?php } } } else { ?>
                        <li><?php echo $this->lang->line('knowledge_topic_not_found'); ?></li>
                    <?php } ?>
                </ul>
            <?php }} else { ?>
            <li><?php echo $this->lang->line('knowledge_topic_not_found'); ?></li>
            <?php } ?>
        </ul>
    </div>
</div>