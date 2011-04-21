<div>
    <h1><b>Insight, leads and perspectives:</b></h1>
    <ul>
        <?php foreach ((array)$topicLeads as $lead) {
        ?>
            <li><?php echo $topicDetail['topic']; ?> "<?php echo $lead['assoc']; ?>"</li>
            <ul>
            <?php foreach ((array)$lead['topics'] as $l) { ?>
                <li>
                <?php
                $split = explode('::', $l['topic']);
                if (is_array($split) && !empty($split) && $split[0] != 'any') {
                    $output = sprintf("<a href='/welcome/knowledge/pid/%s'>%s </a> ( in %s )", $l['id'], $split[1], $split[0]);
                } else if (is_array($split) && !empty($split) && $split[0] == 'any') {
                    $output = sprintf("<a href='/welcome/knowledge/pid/%s'>%s </a>", $l['id'], $split['1']);
                }
                echo $output;
                ?>
            </li>
            <?php } ?>
        </ul>
        <?php } ?>
    </ul>
</div>