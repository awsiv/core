<div style="margin-left: 12px;">   
    <h1><b>Other topics mentioned in the context of "<?php echo $topicDetail['topic']; ?>/<?php echo $topicDetail['context']; ?>":</b></h1>
    <?php //echo var_dump($topicCategory); ?>
    <ul style="padding: 15px;">
        <li> <?php echo sprintf('<a href="%s/knowledge/knowledgemap/pid/%s">%s</a>',site_url(), $topicCategory['topic']['id'], $topicCategory['topic']['name']); ?></li>
        <?php if (is_array($topicCategory['topic']['sub_topics']) && !empty($topicCategory['topic']['sub_topics'])) {
        ?>
            <ul>
            <?php foreach ((array) $topicCategory['topic']['sub_topics'] as $subtopic) {
            ?>
                <li> <?php echo sprintf('<a href="%s/knowledge/knowledgemap/pid/%s">%s</a>',site_url(), $subtopic['id'], $subtopic['topic']); ?></li>
            <?php } ?>
        </ul>
        <?php } ?>
    </ul>
</div>