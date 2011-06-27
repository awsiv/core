<div style="margin-left: 12px;">
<h2>References to "<?php echo $topicDetail['topic']; ?>" in the context of "<?php echo $topicDetail['context']; ?>"</h2>
<ul  style="padding: 15px;">
    <?php foreach ((array)$topicHits as $topic) {
    ?>

    <?php if ($topic['represents'] == 'description') { ?>
            <li><?php echo sprintf("%s :: %s (%s) ", $topic['context'], $topic['ref'], $topic['represents']); ?></li>
    <?php } else {?>
            <li><?php echo sprintf("%s:: <a href='%s'> %s</a> (URL)", $topic['context'], $topic['ref'], $topic['represents']); ?></li>
    <?php } ?>
<?php } ?>
</ul>
</div>