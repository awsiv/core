
<?php
$path = isset($url) ? $url : 'search/index/';
?>
<div class="Paging">
    <div class="pages">
        <div class="inside">
            <?php if($pg['page']> $pg['first'] && $pg['start'] >  $pg['first']){?>
            <a href="<?php echo site_url($path . $params . 'rows/' . $number_of_rows . '/page/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
            <?php }?>
            
            <?php if($pg['page']> $pg['first'] && $pg['start'] >  $pg['first']){?>
            <a href="<?php echo site_url($path . $params . 'rows/' . $number_of_rows . '/page/' .$pg['prevSet'] ) ?>" title="Go to Previous Page <?php echo $pg['prevSet'] ?>" class="prev"><span><</span></a>
            <?php }?>
            
            <?php
            for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                if ($i == $pg['page']) {
                    $current_class = 'current';
                } else {
                    $current_class = "";
                }
                ?>

                <a href="<?php echo site_url($path . $params . 'rows/' . $number_of_rows . "/page/$i") ?>" title="Go to Page <?php echo $i ?>" class="page <?php echo $current_class; ?>"><span><?php echo $i; ?></span></a>

            <?php } ?>
            
             <?php if($pg['page'] != $pg['last'] && $pg['end']< $pg['last']){?>
            <a href="<?php echo site_url($path . $params . 'rows/' . $number_of_rows . '/page/' . $pg['nextSet']) ?>" title="Go to page <?php echo $pg['nextSet'] ?>" class="next"><span>></span></a>
             <?php } ?>
            
            <?php if($pg['page'] != $pg['last'] && $pg['end']< $pg['last']){?>
            <a href="<?php echo site_url($path . $params . 'rows/' . $number_of_rows . '/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
            <?php } ?>
            
           <div class="page_counter">
        <?php
        echo form_open($path . $params);
        echo form_input('rows', $number_of_rows);
        echo "<span>  Rows/Page</span>";
        echo form_close();
        ?>
    </div>
        </div>
         
    </div>
</div>
