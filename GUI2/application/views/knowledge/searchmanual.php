<div id="body">
    <div class="outerdiv">

        <div class="innerdiv" style="min-width: 500px;">
            <h1>Results for search :  <?php echo $search; ?></h1>
            <?php if (is_array($searchData) && !empty($searchData)) { ?>    
            <ul style="padding: 5px;" class="knowledgesearch">
                    <?php foreach ((array) $searchData as $item) { ?>
                        <?php if (trim($item['represents'])) { ?>
                <li style="padding:5px;"><?php echo sprintf('"<a target="_self" href="%s">%s</a>" is mentioned in the context of %s',  trim(base_url(),'/').$item['ref'], $item['represents'], $item['context']); ?></li>
                        <?php } ?>
                    <?php } ?>
                </ul>
            <?php } else if ($search) { ?>
                <div>No topics found.</div> 
            <?php } ?>
        </div>
    </div>
</div>
