<?php
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
//var_dump($searchData);
?>
<div>
    <ul>
    <?php foreach ((array) $searchData as $item) { ?>
        <?php if (trim($item['topic'])) { ?>
        <li><?php echo sprintf('"<a href="/knowledge/knowledgemap/pid/%s">%s</a>" is mentioned in the context of %s',$item['id'],$item['topic'],$item['context']); ?></li>
        <?php } ?>
    <?php } ?>
    </ul>
</div>
