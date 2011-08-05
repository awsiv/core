<div id="body" class="grid_12 outerdiv">
    <div class="innerdiv">
        <div class="push_3 grid_9" style="margin-top:20px;margin-bottom:20px;">
            <div style="margin:20px;">    
                <h2>Welcome to the Library!</h2> 
            </div>
            <div id="library" class="dash" style="margin-top:40px;">
                <ul>
                    <li><?php echo anchor('/knowledge/docs/', 'Docs', array('class' => 'docs showqtip', 'id' => 'docs','title'=> tooltip('tooltip_docs'))); ?></li>
                    <li><?php echo anchor('/knowledge/topicFinder/', 'Find Topic', array('class' => 'topic showqtip' ,'title'=> tooltip('tooltip_find_topics'),'id' => 'topic')); ?></li>
                    <li><?php echo anchor('/notes/shownotes/', 'Notes Archive', array('class' => 'knowledge-message showqtip', 'id' => 'knowledge-message','title'=> tooltip('tooltip_notes_archive'))); ?></li>
                    <li><?php echo anchor('http://cfengine.com/pages/community', 'Community', array('target'=>'_blank','class' => 'forum showqtip', 'id' => 'forum','title'=> tooltip('tooltip_community'))); ?></li>
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