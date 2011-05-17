<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
         
     <div id="body" class="outerdiv grid_12">
                <div id="colorhostlist"class="innerdiv tables">
                    <p class="hostlisttitle">
                    <span class="<?php echo $type.'host'?> coloriconinline"></span><span class="title"><?php echo $type?> hosts</span>
                    </p>

      <?php
      if($count>0)
      {
      echo $tabledata;
                $pg = paging($current, $number_of_rows, $count, 10);?>
                <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?= site_url('welcome/hosts/'.$type.'/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?= site_url('welcome/hosts/'.$type.'/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $selected = 'current'; else
                                $selected="";
                        ?>

                            <a href="<?= site_url("welcome/hosts/$type/rows/$number_of_rows/page/$i") ?>" title="Go to Page <?= $i ?>" class="page <?= $selected ?>"><span><?= $i ?></span></a>

                        <? } ?>

                        <a href="<?= site_url("welcome/hosts/$type/rows/$number_of_rows/page/" . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?= site_url("welcome/hosts/$type/rows/$number_of_rows/page/" . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>
                <div>
                    <?php
                    echo form_open('welcome/hosts/'.$type);
                    echo form_input('rows', $number_of_rows);
                    echo "Rows/Page";
                    echo form_close();
                    ?>
                </div>
            </div>
       <?php }

       else
       {
         echo "No data Found";
       }
       ?>
</div>
</div>