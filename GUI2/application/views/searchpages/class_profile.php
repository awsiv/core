<div class="grid_5">
<div class="panel"><div class="panelhead"><?php echo $report_type?> query</div>
  <div class="panelcontent">
         <form method="post" action="<?php echo site_url('search')?>">
         <p>Class pattern: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <input type="hidden" name="manyhosts" value="true">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input type="submit" value="Generate report"></p>
          </form>
	</div>
</div>
</div>
<div class="grid_7"><div id="tips"><img src="<?php echo get_imagedir()?>Lamp.png" />
    <p> Use regular expressions that match the <b>whole string</b> you want to find
    <a href="knowledge.php?topic=regular expression">Learn about regular expressions</a></p>
   <div class="clearleft"></div></div></div>
<div class="clear"></div>