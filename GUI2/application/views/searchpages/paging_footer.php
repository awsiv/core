
             <div class="Paging">
                    <div class="pages">
                        <div class="inside">
                            <a href="<?php echo site_url('search/index/' . $params . 'rows/' . $number_of_rows . '/page/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                            <a href="<?php echo site_url('search/index/' . $params . 'rows/' . $number_of_rows . '/page/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

            <?php
                for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                    if ($i == $pg['page'])
                        $current = 'current'; else
                        $current="";
            ?>

                        <a href="<?php echo site_url("search/index/" . $params .'rows/' . $number_of_rows . "/page/$i") ?>" title="Go to Page <?php echo  $i ?>" class="page <?php echo  $current; ?>"><span><?php echo  $i; ?></span></a>

                    <? } ?>

                        <a href="<?php echo site_url('search/index/' . $params . 'rows/' . $number_of_rows . '/page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo site_url('search/index/' . $params . 'rows/' . $number_of_rows . '/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>

            </div>
           <div>
            <?php
                echo form_open('search/index/' . $params);
                echo form_input('rows', $number_of_rows);
                echo "  Rows/Page";
                echo form_close();
            ?>
       </div>