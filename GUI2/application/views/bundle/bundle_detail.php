<div class="grid_5">
           <div class="panel">
             <div class="panelhead">ALL BUNDLES</div>
              <div class="panelcontent">
               <?php echo $allbundles?>
              </div>
            </div>
          </div>
      <div class="grid_7">
        <div class="panel">
	      <div class="panelhead">Bundle definition of <?php echo $bundle?>:</div>
            <div class="panelcontent">
			<ul>
			<li><i>Name:</i> <span id="bundle"><?php echo $bundle?></span><br></li>
			<li><i>Arguments:</i> <span id="args"><?php echo $args?></span></li>
			<li><i>Host classes using this bundle:</i> <?php echo $classes?></li>
			<li><i>Promises in this bundle:</i> <?php echo $list?></li>
			<li><i>Bundles using this bundle:</i><?php echo $others?></li>
			</ul>
            </div>
          </div>
        </div>
  <div class="clear"></div>
