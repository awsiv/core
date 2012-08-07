<div style="padding: 20px;">
<h1>Stories about <?php echo $topicDetail['topic']; ?></h1>
<?php foreach ((array)$stories as $index => $mainStory)
{ //var_dump($mainStory);  ?>
<h2>Story <?php echo $index+1;?></h2>

    <ul class="<?php echo $index ?>">
        <?php
        foreach ((array) $mainStory['story'] as $i => $s)
        {
            ?>
            <?php
            $marginLeft = ( $s['intro'] === 'and') ? 30 : 10;
            $url = site_url('knowledge/knowledgemap') . '/pid/' . $s['topic_id'];
            $t = sprintf('<a href="%s">%s</a>', $url, $s['topic']);
            ?>
            <li style="margin-left: <?php echo $marginLeft; ?>px;"><?php echo sprintf('%s %s %s in %s', $s['intro'], $s['association'], $t, $s['context']); ?></li>
            <?php
        }
        ?>

        <?php
        foreach ((array) $mainStory['notes'] as $i => $n)
        {

            ?>
            <?php
            $url = site_url('knowledge/knowledgemap') . '/pid/' . $n['topic_id'];
            $t = sprintf('<a href="%s">%s</a>', $url, $n['topic']);
            ?>
            <li style="margin-left: 10px;">Note this <?php echo sprintf('%s %s in %s', $n['assoc'], $t, $n['context']); ?></li>
            <?php
        }
        ?>


    </ul>
<?php } ?>
</div>
