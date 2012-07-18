<div id="footer">
    <p class="grid_7"><?php echo CFE_PRODUCT_NAME .' '. CFE_HUB_VERSION; ?> running on <?php echo CFE_CORE_VERSION; ?> </p>
    <p class="grid_5 alignright">Copyright &copy; 2012 CFEngine AS - All rights reserved. (<a href="http://cfengine.com/commercial-license" target="_blank">License information</a>)</p>
    <p class="clear"></p>
</div>
</div>
<script type="text/javascript">
    // q tip initialization 
    $(document).ready(function() {
        
        <?php if ($this->setting_lib->get_tooltips_status()) { ?> 
        
        $('.showqtip,a.help').live('mouseover', function(event) {
            // Bind the qTip within the event handler

            var pointer=$(this).data('my')?$(this).data('my'):'top center';
            var body=$(this).data('at')?$(this).data('at'):'bottom center';
            $(this).qtip({
                position: {
                    my:pointer,
                    at:body
                },
                show: {
                    event: event.type, // Use the same show event as the one that triggered the event handler
                    ready: true // Show the tooltip as soon as it's bound, vital so it shows up the first time you hover!
                },
              style: {
	            classes: 'ui-tooltip-shadow ui-tooltip-' + 'dark'
	      }
            }, event); // Pass through our original event to qTip
        })
        
         <?php } ?>   
             
             $('a.searchsubmit').live('click',function(){
                 $(this).parents("form").submit();
                 return false; 
             })
    });
</script>
</body>
</html>
