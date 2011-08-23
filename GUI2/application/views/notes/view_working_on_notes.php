<div class="outerdiv grid_12">
    <div class="innerdiv">
        <p class="title">My Work Notes</p>
     <?php
       $this->table->set_heading('Notes', 'Date');
        foreach($table as $rows)
        {
            $this->table->add_row($rows['working_on'], date('D F d h:i:s Y',$rows['date']));
        }
        echo "<div class=\"tables\">".$this->table->generate()."</div>";
       $post_url='welcome/workingNotes';
       $url='welcome/workingNotes/rows/'.$rows_per_page;
       $pg = paging($current_page,$rows_per_page, $total, 10);
      ?>
              <div class="Paging">
                    <div class="pages">
                        <div class="inside">
                            <a href="<?php echo site_url($url.'/page/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                            <a href="<?php echo site_url($url. '/page/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

            <?php
                for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                    if ($i == $pg['page'])
                        $current = 'current'; else
                        $current="";
            ?>

                        <a href="<?php echo site_url($url. "/page/$i") ?>" title="Go to Page <?php echo  $i ?>" class="page <?php echo  $current; ?>"><span><?php echo  $i; ?></span></a>

                    <?php } ?>

                        <a href="<?php echo site_url($url. '/page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo site_url($url. '/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>

            </div>
           <div>
            <?php
                echo form_open($post_url);
                echo form_input('rows', $rows_per_page);
                echo "  Rows/Page";
                echo form_close();
            ?>
       </div>
    </div>
</div>

