<div style="margin-left: 12px;">
    <h2>References to "<?php echo $topicDetail['topic']; ?>" in the context of "<?php echo $topicDetail['context']; ?>"</h2>
    <?php if (isset ($topicHits) && !empty($topicHits) ) { ?>
    <table class="bundlelist-table">
        <tr>
            <th scope="col">Context</th>
            <th scope="col">Results</th>
        </tr>
        <?php foreach ((array) $topicHits as $topic) { ?>
            <tr>
                <td>
                    <?php echo $topic['context']; ?>
                </td>
                <td>
                    <?php
                    if ($topic['represents'] == 'description') {
                        echo sprintf("%s (%s) ", $topic['ref'], $topic['represents']);
                    } else {
                        echo sprintf("<a href='%s'> %s</a> (URL)", $topic['ref'], $topic['represents']);
                    }
                    ?>
                </td>
            </tr>
        <?php } ?>
    </table> 
<?php } else { ?>
    <div>No information found on this topic.</div>
<?php } ?>
</div>