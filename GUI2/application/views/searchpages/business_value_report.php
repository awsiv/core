
 <div id="searchform" class="panel">
   <div class="panelhead"><?php echo $report_type?> query</div><div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Date:<input class="searchfield" type="text" name="days" ></p>
          <p>Months:<input class="searchfield" type="text" name="months" ></p>
          <p>Years:<input class="searchfield" type="text" name="years" ></p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <input type="hidden" name="host" value="All"">
          <input type="hidden" name="report" value="<?php echo $report_type ?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
         <p><input class="btn" type="submit" value="Generate report"></p>
   </form>
   </div>
 </div>
