<div id="searchform" class="panel">
   <div class="panelcontent">
     <form method="post" action="<?php echo site_url('search')?>">
         <p>Bundle pattern: (.*+[])<input class="searchfield" type="text" name="name" id="bp" value="<?php echo (is_array($paramArray) && isset($paramArray['name'])) ? trim($paramArray['name']) : ''; ?>"></p>
         
         <!--<p>Host class: (.*+[]) 
         <a href="<?php echo site_url('widget/contextfinder')?>" id="bphghelp" class="help" title="<?php echo $this->lang->line('report_hostgp_help');?>">Help ?</a>
         <input class="searchfield" type="text" name="class_regex" size="80" id="hg" value="<?php echo (is_array($paramArray) && isset($paramArray['class_regex'])) ? trim($paramArray['class_regex']) : ''; ?>"></p>-->
         <p>
             <a href="<?php echo site_url('widget/contextfinder')?>" id="hclist" class="hostcontextddl floatleft" title="<?php echo $this->lang->line('report_hostgp_help');?>">Hostcontext</a>
             <span class="floatright"> Return host names only: <input type="checkbox" name="hosts_only" value="true"></span>
             <span class="clear"></span>
         </p>
         
    
         <p class="alignrightbuttoncontainer">
             <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
            
         </p>
         <input type="hidden" name="host" value="All">
         <input type="hidden" name="inclist" value="<?php echo (is_array($paramArray) && isset($paramArray['inclist'])) ? trim($paramArray['inclist']) : ''; ?>">
         <input type="hidden" name="exlist" value="<?php echo (is_array($paramArray) && isset($paramArray['exlist'])) ? trim($paramArray['exlist']) : ''; ?>">
         <input type="hidden" name="report" value="<?php echo $report_type?>">
     </form>
   </div>
   
</div>
<script type="text/javascript">

 var $incList=$('input:hidden[name=inclist]');
 var $exList=$('input:hidden[name=exlist]');
 $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});
 //$('#hg').smartTextBox({separator : "|",editOnFocus:true});

  
            //baseUrl: '<?php echo site_url() ?>'
      var $cont= $('#hclist').contextfinder({
          baseUrl: '<?php echo site_url() ?>',
          complete:function(event,data){
               $incList.val(data.includes);
               $exList.val(data.excludes);
             
          }
        });   
  
  //$('#hclist').contextfinder('setContext',$incList.val().split(','), $exList.val().split(','));
var obj = { name: 'filterview' };
mediator.installTo(obj);
 obj.subscribe('contextChange', function(data){
        console.log(data);
        $incList.val(data.includes);
        $exList.val(data.excludes);
 });
obj.publish('contextChange', $incList.val().split(','))

  /*$('#searchform').live('submit',function(){
      var list=$('#hg').data('list');
      console.log($('input:hidden[name=inclist]').val());
   return false;
      
  })*/
</script>
