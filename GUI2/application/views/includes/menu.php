 <div id="nav">
             <ul class="grid_9">
             <?php if(isset($breadcrumbs)): ?>
                 <?php echo $breadcrumbs?>
              <?php endif;?>

             </ul>
     
     <div id="searcharea" class="grid_2 alignright">
        <form action="<?php echo site_url('welcome/knowledge')?>" method="post">
          <label id="searchbox">
              <input type="text" name="search" value="Search in knowledge map" onfocus="if(this.value==this.defaultValue)this.value='';" onblur="if(this.value=='')this.value=this.defaultValue;"/>
              <input type="submit" class="hidden" value="Search"/>
          </label>
         </form>
      </div>
    <div class="clearleft"></div>
 </div>