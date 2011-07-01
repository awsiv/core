<div id="body" class="grid_12 outerdiv">
    <div class="innerdiv">
        <div class="push_3 grid_9" style="margin-top:20px;margin-bottom:20px;">
            <div style="margin:20px;">    
                <h2>Welcome to the Library!</h2> 
            </div>
            <div id="library" class="dash" style="margin-top:40px;">
                <ul>
                    <li><?php echo anchor('/knowledge/docs/', 'Docs', array('class' => 'docs', 'id' => 'docs')); ?></li>
                    <li><?php echo anchor('/knowledge/topicFinder/', 'Find Topic', array('class' => 'topic' ,'title'=>'Topic Finder','id' => 'topic')); ?></li>
                    <li><?php echo anchor('/notes/shownotes/', 'Message Archive', array('class' => 'knowledge-message', 'id' => 'knowledge-message')); ?></li>
                    <li><?php echo anchor('http://cfengine.com/pages/community', 'Community', array('class' => 'forum', 'id' => 'forum')); ?></li>
                </ul>
            </div>
            <div class="clearboth"></div>
        </div>   
        <div class="clearboth"></div>        
    </div>
</div>
<script type="text/javascript">
    jQuery(document).ready(function(){
      
       $('#topic').topicfinder();       

    });
</script>