
<div style="margin-left: 12px;">   
    <h2>Similar topics to "<?php echo $topicDetail['topic']; ?>" referred in the same context "<?php echo $topicDetail['context']; ?>"</h2>
    <ul  style="padding: 15px;">
        <?php foreach ((array) $topicCategory['other_topics'] as $others) {
        ?>
            <li> <?php echo sprintf('<a href="/knowledge/knowledgemap/pid/%s">%s</a>', $others['id'], $others['topic']); ?></li>
        <?php } ?>
    </ul>

</div>