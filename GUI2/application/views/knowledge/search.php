<?php
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
?>
<?php if (isset($showSearchBar)) { ?>
    <div>
        <form method="post" action="/knowledge/topicFinder/">
            Search :: <input name="search" value="<?php echo $search; ?>"></input>             
        </form>     
    </div>
<?php } ?>
<div>
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
