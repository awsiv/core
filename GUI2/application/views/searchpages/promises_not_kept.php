 <div id="searchform" class="panel">
   <div class="panelhead"><?php echo $report_type ?> query</div>
     <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Promise handles: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <p>From minimum age <input type="text" name="hours_deltafrom" size="2"> hours to maximum age <input type="text" name="hours_deltato" size="2"> hours old. </p>
          <input type="hidden" name="host" value="All">
          <input type="hidden" name="report" value="<?php echo $report_type ?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input class="btn" type="submit" value="Generate report"></p>
          </form>
      </div>
  </div>