
 <div id="searchform" class="panel"><div class="panelhead"><?php echo $report_type?> query</div><div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Promise by handle: (.*+[])<input class="searchfield" type="text" name="name"></p>
          <p><select name="state"><option value="x">Any</option>
		     <option value="c">Compliant</option>
			 <option value="r">Repaired</option>
			 <option value="n">Non-compliant</option>
			 </select>
		  </p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <input type="hidden" name="manyhosts" value="true">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input type="submit" value="Generate report"></p>
          </form>
 </div>