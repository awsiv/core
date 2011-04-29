 <div id="searchform" class="panel"><div class="panelhead"><?php echo $report_type?> query</div>
 <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Host name: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>Host address: (.*+[])<input class="searchfield" type="text" name="address"></p>
          <p>Host's unique key:<input class="searchfield" type="text" name="key"></p>
          <p>Hours ago:<p><input class="searchfield" type="text" name="ago"></p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <!--<input type="hidden" name="manyhosts" value="true">-->
          <input type="hidden" name="host" value="All">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input class="btn" type="submit" value="Generate report"></p>
   </form>
 </div>
 </div>