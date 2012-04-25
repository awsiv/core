<?php $other_topic_total = count($topicCategory['other_topics']); ?>

<div style="margin-left: 12px;">   
    <h2>Similar topics to "<?php echo $topicDetail['topic']; ?>" referred in the same context "<?php echo $topicDetail['context']; ?>"</h2>
    <ul  style="padding: 15px;">
        <?php
            $offset = ($currentPage-1) * $number_of_rows;
            $tmp_arr = array_splice($topicCategory['other_topics'], $offset, $number_of_rows);
            foreach ((array) $tmp_arr as $others) {
        ?>
        <li> <?php echo sprintf('<a href="%s/knowledge/knowledgemap/pid/%s">%s</a>',  site_url(), $others['id'], $others['topic']);?></li>
        <?php } ?>
    </ul>

    <?php $pg = paging($currentPage, $number_of_rows, $other_topic_total, 10); ?>

    <div class="Paging">
        <div class="pages">
            <div class="inside">
                <a href="<?php echo site_url($pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['first'] .'#tabs-4'); ?>" title="Go to First Page" class="first"><span>First</span></a>
                <a href="<?php echo site_url($pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['prev']) .'#tabs-4'; ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                <?php
                    for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                        if ($i == $pg['page'])
                            $current = 'current'; 
                        else
                            $current = "";
                ?>

                <a href="<?php echo site_url($pagingParam. "/rows/" . $number_of_rows."/page/$i" .'#tabs-4') ?>" title="Go to Page <?php echo $i; ?>" class="page <?php echo $current; ?>"><span><?php echo $i; ?></span></a>

                <?php } ?>

                <a href="<?php echo site_url($pagingParam.'/rows/' . $number_of_rows . '/page/' . $pg['next'] .'#tabs-4') ?>" title="Go to Next Page" class="next"><span>></span></a>
                <a href="<?php echo site_url($pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['last'] .'#tabs-4') ?>" title="Go to Last Page" class="last"><span>Last</span></a>
            </div>
        </div>

    </div>
    
</div>