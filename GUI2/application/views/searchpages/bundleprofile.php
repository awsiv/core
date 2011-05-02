
<div id="searchform" class="panel">
   <div class="panelhead"><?php echo $report_type.' query'?></div><div class="panelcontent">
     <form method="post" action="<?php echo site_url('search')?>">
         <p>Bundle pattern: (.*+[])<input class="searchfield" type="text" name="name" id="bp" > <a href="#" id="bphelp">Help ?</a></p>
         <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80" id="hg"><a href="#" id="bphghelp">Help ?</a></p>
         <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
         <p><input class="btn" type="submit" value="Generate report"></p>
         <input type="hidden" name="host" value="All">
         <input type="hidden" name="report" value="<?php echo $report_type?>">
     </form>
   </div>
</div>
<script type="text/javascript">
$('#bphghelp').classfinder();
</script>