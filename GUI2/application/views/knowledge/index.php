<div id="body" class="grid_12 outerdiv">
    <div class="innerdiv">
        <div class="push_3 grid_9" style="margin-top:20px;margin-bottom:20px;">
            <div style="margin:20px;">    
                <h2>Welcome to the Library!</h2> 
                <div>Todays message is that this library page is in construction. May need more modifications. Stay tuned.</div>   
            </div>
            <div id="library" class="dash" style="margin-top:40px;">
                <ul>
                    <li><a href="/docs/cf3-reference.html" class="docs" id="docs">docs</a></li>
                    <li><?php echo anchor('/knowledge/topicFinder/', 'Find Topic', array('class' => 'topic' ,'title'=>'Topic Finder','id' => 'topic')); ?></li>
                    <li><?php echo anchor('/notes/shownotes/', 'Message Archieve', array('class' => 'message', 'id' => 'message')); ?></li>
                    <li><?php echo anchor('#', 'Forum', array('class' => 'forum', 'id' => 'forum')); ?></li>
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
       // $('#topic').ajaxyDialog({title:'Topic Finder'});

    });
</script>