 <div id="nav">
             <ul class="grid_10">
             <li><a href="<?php echo site_url()?>/welcome/index" class="<?php $text=isset($summary)?$summary:""; echo $text ?>">SUMMARY</a></li>
			 <li><a href="<?php echo site_url()?>/welcome/status" class="<?php $text=isset($status)?$status:""; echo $text ?>">STATUS</a></li>
             <li><a href="<?php echo site_url()?>/welcome/helm" class="<?php $text=isset($planning)?$planning:""; echo $text ?>">PLANNING</a></li>
             <li><a href="<?php echo site_url()?>/welcome/knowledge" class="<?php $text=isset($library)?$library:""; echo $text ?>">KNOWLEDGE</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             <?php $text=isset($nav_text)?$nav_text:""; echo $text?> 
             </span>
             <div class="clearleft"></div>
 </div>