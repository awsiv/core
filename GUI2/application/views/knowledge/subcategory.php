<div style="margin-left: 12px;">   
    <ul  style="padding: 15px;">
        <?php foreach ((array) $topicCategory['other_topics'] as $others) {
        ?>
            <li> <?php echo sprintf('<a href="/knowledge/knowledgemap/pid/%s">%s</a>', $others['id'], $others['topic']); ?></li>
        <?php } ?>
    </ul>

</div>