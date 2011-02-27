 <div id="nav">
             <ul class="grid_9">
             <?php if(isset($breadcrumbs)): ?>
                 <?=$breadcrumbs?>
              <?php endif;?>

             </ul>
     
     <div id="searcharea" class="grid_2 alignright">
        <form action="<?php echo site_url('welcome/knowledge')?>" method="post">
          <label id="searchbox">
              <input type="text" name="search" /><input type="submit" class="hidden" value="Search"/>
          </label>
         </form>
      </div>
    <div class="clearleft"></div>


 </div>