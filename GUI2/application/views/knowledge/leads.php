<div>
    <h1><b>Insight, leads and perspectives:</b></h1>
    <div style="margin-left: 12px;">
        <ul>
            <?php foreach ((array) $topicLeads as $lead) {
                ?>
            <li style="list-style: none;"><span style="color: #212121;"><?php echo $topicDetail['topic']; ?> "<?php echo $lead['assoc']; ?>"</span></li>
                <ul style="padding: 15px;">
                    <?php foreach ((array) $lead['topics'] as $l) { ?>
                        <li>
                            <?php
                            $split = explode('::', $l['topic']);
                            if (is_array($split) && !empty($split) && $split[0] != 'any') {
                                $output = sprintf("<a href='/knowledge/knowledgemap/pid/%s'>%s </a> ( in %s )", $l['id'], $split[1], $split[0]);
                            } else if (is_array($split) && !empty($split) && $split[0] == 'any') {
                                $output = sprintf("<a href='/knowledge/knowledgemap/pid/%s'>%s </a>", $l['id'], $split['1']);
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