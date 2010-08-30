<div class="container_16" id="reports">
 <div id="tabs" class="grid_16">
	  <ul>
	   <li><a href="#MissionOverview">Mission overview</a></li>
	   <li><a href="<?php echo site_url('policy/test')?>">Mission Status</a></li>
	   <li><a href="<?php echo site_url('policy/missioncontrol')?>">Mission Control</a></li>
	   <li><a href="#MissionIntegrity">Mission Integrity</a></li>
	   <li><a href="#Knowledge">Knowledge</a></li>
	   <li><a href="#Missionlog">Mission log</a></li>
	 </ul>
	  
	 <div id="MissionOverview" class='grid_14'>
	 <div id="curstat" class="panels">
		  <fieldset>
		  <legend>Current status</legend>
		  </fieldset> 
    </div>
    <div id="compliance" class="panels">
		  <fieldset>
		  <legend>Compliance levels</legend>
		  </fieldset> 
    </div>
    
    <div class="clear">
    </div>
    </div>
 </div>
</div>
<script type="text/javascript">
$(document).ready(function() {
	var $tabs = $('#tabs').tabs();

	$('.panels').resizable({
		resize: function(e, ui) {
		$('fieldset',this).css('height',ui.size.height-10);
		$('fieldset',this).css('width',ui.size.width-10);
		   }    
	});
	
	$('#curstat').parent().draggable();
	$('#compliance').parent().draggable();
  });
 </script>