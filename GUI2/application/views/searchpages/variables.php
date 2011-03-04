<div id="searchform" class="panel">
  <div class="panelhead"><?php echo $report_type?> query</div>
   <div class="panelcontent">
        <form method="post" action="<?php echo site_url('search')?>">
          <p>Scope or bundle: (.*+[])<input class="searchfield" type="text" name="scope"></p>
          <p>Lvalue or name: (.*+[])<input class="searchfield" type="text" name="lval"></p>
          <p>Rvalue or content: (.*+[])<input class="searchfield" type="text" name="rval"></p>
          <p>Type:<p>
          <select name="type"><option value="s">String</option><option value="sl">String list</option><option value="i">Integer</option><option value="il">Integer list</option><option value="r">Real</option><option value="rl">Real list</option></select>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <input type="hidden" name="manyhosts" value="true">
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input class="btn" type="submit" value="Generate report"></p>
          </form>
      </div>
   </div>
