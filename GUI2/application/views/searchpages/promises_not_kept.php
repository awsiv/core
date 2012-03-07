 <div id="searchform" class="panel">
   <div class="panelhead"><?php echo $report_type ?> query</div>
     <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>Promise handles: (.*+[])<input class="searchfield" type="text" name="name" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
     
          <div id="morefilters" style="display:none">
          <p>From minimum age <input type="text" name="hours_deltafrom" size="2" value="<?php echo (is_array($paramArray) && isset($paramArray['hours_deltafrom'])) ? trim($paramArray['hours_deltafrom']) : ''; ?>"> hours to maximum age <input type="text" name="hours_deltato" size="2" value="<?php echo (is_array($paramArray) && isset($paramArray['hours_deltato'])) ? trim($paramArray['hours_deltato']) : ''; ?>" > hours old. </p>
          </div>
          
           <p>
             <a href="<?php echo site_url('widget/contextfinder')?>" id="hclist" class="hostcontextddl floatleft" title="<?php echo $this->lang->line('report_hostgp_help');?>">Hostcontext</a>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
          </p>
          
           <p class="alignrightbuttoncontainer">
             <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
             <a  id="morefiltertoggle"class="green_btn" href="#"><span>More Filters</span></a>
          </p>
          
          <input type="hidden" name="host" value="All">
          <input type="hidden" name="inclist" value="<?php echo (is_array($paramArray) && isset($paramArray['inclist'])) ? trim($paramArray['inclist']) : ''; ?>">
          <input type="hidden" name="exlist" value="<?php echo (is_array($paramArray) && isset($paramArray['exlist'])) ? trim($paramArray['exlist']) : ''; ?>">
          <input type="hidden" name="report" value="<?php echo $report_type ?>">

          </form>
      </div>
  </div>
<script type="text/javascript">
 var $incList=$('input:hidden[name=inclist]');
 var $exList=$('input:hidden[name=exlist]');
 
 $('#hclist').contextfinder({
          baseUrl: '<?php echo site_url() ?>',
          complete:function(event,data){
              $('#hg').data('list',data.inccl).val('includes='+data.inccl.incList+' exclude='+data.inccl.exList);
               $incList.val(data.inccl.incList);
               $exList.val(data.inccl.exList);
          }
  }); 

$('#morefiltertoggle').live('click',function(){
     $('#morefilters').toggle();
});
</script>
