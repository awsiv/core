<div id="body" class="grid_12">
    <div class="outerdiv">

        <div class="innerdiv">
            <h1>Results for search :  <?php echo $search; ?></h1>
            <?php if (is_array($searchData) && !empty($searchData)) { ?>    
                <ul>
                    <?php foreach ((array) $searchData as $item) { ?>
                        <?php if (trim($item['topic'])) { ?>

                            <li><?php echo sprintf('"<a target="_self" href="/knowledge/knowledgemap/pid/%s">%s</a>" is mentioned in the context of %s', $item['id'], $item['topic'], $item['context']); ?></li>

                        <?php } ?>
                    <?php } ?>
                </ul>
            <?php } else if ($search) { ?>
                <div>No topics found.</div> 
            <?php } ?>
        </div>
    </div>
</div>
