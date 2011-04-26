<div id="body">
    <div class="left"></div>
    <div class="middle minwidth98">
      <div id="finder" class="dash innerdiv">
       <p class="title">Finder</p>
              <ul>
                          <li><?php echo anchor('#','host',array('class'=>'host','id'=>'findhost'));?></li>
                          <li><?php echo anchor('widget/allclasses','class',array('class'=>'classes','id'=>'findclass'));?></li>
                          <li><?php echo anchor('widget/allpolicies','policy',array('class'=>'policies','id'=>'findpolicy'));?></li>
                          <li><?php echo anchor('widget/allreports','reports',array('class'=>'report','id'=>'findreport'));?></li>
                          <p class="clearleft"></p>
              </ul>
          
      </div>
    </div>
    <div class="right"></div>
    <div class="clearboth"></div>
    </div>
 <div class="clear"></div>
 
<script type="text/javascript">
 $(document).ready(function(){
 $('#findhost').hostfinder();
 $('#findclass').classfinder();
 $('#findpolicy').policyfinder();
 $('#findreport').reportfinder();
 });
</script>