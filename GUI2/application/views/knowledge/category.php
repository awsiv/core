<div>   
    <h1><b>Other topics mentioned in the context of "<?php echo $topicDetail['topic']; ?>/<?php echo $topicDetail['context']; ?>":</b></h1>
    <?php //echo var_dump($topicCategory); ?>
    <ul>
            <li> <?php echo sprintf('<a href="/welcome/knowledge/pid/%s">%s</a>', $topicCategory['topic']['id'], $topicCategory['topic']['name']); ?></li>
        <?php if (is_array($topicCategory['topic']['sub_topics']) && !empty($topicCategory['topic']['sub_topics'])) {
        ?>
                <ul>
            <?php foreach ((array) $topicCategory['topic']['sub_topics'] as $subtopic) {
            ?>
                    <li> <?php echo sprintf('<a href="/welcome/knowledge/pid/%s">%s</a>', $subtopic['id'], $subtopic['topic']); ?></li>
            <?php } ?>

            </ul>

        
        <?php } ?>







        <?php foreach ((array) $topicCategory['other_topics'] as $others) {
 ?>
            <li> <?php echo sprintf('<a href="/welcome/knowledge/pid/%s">%s</a>', $others['id'], $others['topic']); ?></li>
<?php } ?>
    </ul>

</div>