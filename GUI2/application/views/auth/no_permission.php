<?php if (is_ajax()) { ?>
        <div id="infoMessage"><?php echo $message;?></div>
<?php } else { ?>
<div class="outerdiv grid_12">
          		
    <div class="innerdiv">
 

        <div class="holder">
            <div id="admin_content" class="tables">
            <div id="infoMessage"><?php echo $message;?></div>
            </div>
        </div>
     </div>
</div>
<?php } ?>