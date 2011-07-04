<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
         
     <div id="body" class="outerdiv grid_12">
                <div id="colorhostlist"class="innerdiv tables">
                   

      <?php
      if($count>0)
      {
          ?>
                     <p class="hostlisttitle">
                    <span class="<?php echo $type.'host'?> coloriconinline"></span><span class="title"><?php echo $type?> hosts</span>
                    </p>
       <?php
      echo $tabledata;
                $pg = paging($current, $number_of_rows, $count, 10);?>
                <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?php echo  site_url('welcome/hosts/'.$type.'/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?php echo  site_url('welcome/hosts/'.$type.'/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $selected = 'current'; else
                                $selected="";
                        ?>

                            <a href="<?php echo  site_url("welcome/hosts/$type/rows/$number_of_rows/page/$i") ?>" title="Go to Page <?php echo  $i ?>" class="page <?php echo  $selected ?>"><span><?php echo  $i ?></span></a>

                        <? } ?>

                        <a href="<?php echo  site_url("welcome/hosts/$type/rows/$number_of_rows/page/" . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo  site_url("welcome/hosts/$type/rows/$number_of_rows/page/" . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
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
         echo "<span class=\"nodata\">".$this->lang->line('no_data')."</span>";
       }
       ?>
</div>
</div>
<script type="text/javascript">
    $(document).ready(function(){
       $('td.actioncol').find('a').qtip({
            style: {
                background:'#454545',
                opacity:0.9,
                name: 'dark',
                width:{
                    min:0,
                    max:150
                },
                border: {
                    width: 1,
                    radius: 1
                },
                tip:{
                    corner:'topMiddle',
                    size:{
                        x:10,
                        y:10
                    }
                }
            },
            show:{
                effect:{
                    type:'fade',
                    lenght:200
                }
            },

            position: {
                corner: {
                    target: 'bottomMiddle',
                    tooltip: 'topMiddle'
                }
            }
        });
    }); 
</script>