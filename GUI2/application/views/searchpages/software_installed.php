<div id="searchform" class="panel">
    <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
              <p><label>Name:</label> <input class="searchfield" type="text" name="name" id="packagename" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
          
          <div id="morefilters" style="display:none">
          <p><label>Version:</label><input class="searchfield" type="text" name="version" size="80" id="version" value="<?php echo (is_array($paramArray) && isset($paramArray['version'])) ? trim($paramArray['version']) : ''; ?>"></p>
          <p><label>Architecture:</label><input class="searchfield" type="text" name="arch" size="80" id="arc" value="<?php echo (is_array($paramArray) && isset($paramArray['arch'])) ? trim($paramArray['arch']) : ''; ?>"></p>
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
          
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <input type="hidden" name="inclist" value="<?php echo (is_array($paramArray) && isset($paramArray['inclist'])) ? trim($paramArray['inclist']) : ''; ?>">
          <input type="hidden" name="exlist" value="<?php echo (is_array($paramArray) && isset($paramArray['exlist'])) ? trim($paramArray['exlist']) : ''; ?>">
          <input type="hidden" name="host" value="All">
         
          </form>
     </div>
  </div>
<script type="text/javascript">
var $incList=$('input:hidden[name=inclist]');
 var $exList=$('input:hidden[name=exlist]');
 
 var $contextfinder=$('#hclist').contextfinder({
          baseUrl: '<?php echo site_url() ?>',
          complete:function(event,data){
               $incList.val(data.includes);
               $exList.val(data.excludes);
          }
  }); 
   
   $('#hclist').contextfinder('setContext',$incList.val().split(','), $exList.val().split(','));
  
$('#morefiltertoggle').live('click',function(){
    var textcontainer=$(this).find('span');
    if( textcontainer.text().toLowerCase()=='more filters'){
       textcontainer.text('Fewer Filters') 
    }else {
        textcontainer.text('More Filters') 
    }
     $('#morefilters').toggle();
});
</script>
