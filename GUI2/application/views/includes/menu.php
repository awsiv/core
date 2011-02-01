 <div id="nav">
             <ul class="grid_10">
             <li><a href="<?php echo site_url()?>/welcome/index" class="<?php $text=isset($summary)?$summary:""; echo $text ?>">SUMMARY</a></li>
			 <li><a href="<?php echo site_url()?>/welcome/status" class="<?php $text=isset($status)?$status:""; echo $text ?>">STATUS</a></li>
             <li><a href="<?php echo site_url()?>/welcome/helm" class="<?php $text=isset($planning)?$planning:""; echo $text ?>">PLANNING</a></li>
             <li><a href="<?php echo site_url()?>/welcome/knowledge" class="<?php $text=isset($library)?$library:""; echo $text ?>">KNOWLEDGE</a></li>
             <?php if(in_array('admin', $this->session->userdata('group') )) {?><li><a href="<?php echo site_url()?>/auth/index" class="<?php $text=isset($admin)?$admin:""; echo $text ?>">ADMIN</a></li><?php }?>
             </ul>
             <span id="status" class="grid_2 alignright">
             <?php //$text=isset($nav_text)?$nav_text:""; echo $text?>	
			 User : <?php echo $this->session->userdata('username');?>
             <a href="<?php echo site_url('auth/logout'); ?>" title="logout"><img src="<?php echo get_imagedir();?>logout.png" class="align" alt="logout"/></a>
             </span>
             <div class="clearleft"></div>
 </div>