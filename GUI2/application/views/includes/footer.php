<div id="footer">
    <p class="grid_7"><?php $product=get_productname(); echo $product['name'].' '.$product['version']?> running on <?php echo cfpr_community_version(); ?> </p>
    <p class="grid_5 alignright">Copyright &copy; 2011 CFEngine AS - All rights reserved. (<a href="http://cfengine.com/commercial-license" target="_blank">License information</a>)</p>
    <p class="clear"></p>
</div>
</div>
<script type="text/javascript">
    // q tip initialization 
    $(document).ready(function() {
        
        <?php if ($this->setting_lib->get_tooltips_status()) { ?> 
        
        $('a.showqtip,a.help').live('mouseover', function(event) {
            // Bind the qTip within the event handler
            $(this).qtip({
                overwrite: false, // Make sure the tooltip won't be overridden once created
                position: {
                    my:'top left',
                    at:'bottom  center'
                },
                show: {
                    event: event.type, // Use the same show event as the one that triggered the event handler
                    ready: true // Show the tooltip as soon as it's bound, vital so it shows up the first time you hover!
                }
            }, event); // Pass through our original event to qTip
        })
        
         <?php } ?> 
        
        
        var dateFormat = 'F d, Y H:i:s'; 
        $('.localtime').each(function(){
            var dateString = $(this).html();
            var localDate = Date.parse(dateString);
            if (localDate) {
                var d = new Date(localDate);
                var offset = -d.getTimezoneOffset()/60;
                if (offset > 0) offset = '+'+offset;
                var dateDisplay = d.format(dateFormat) + ' (GMT' + offset+')';
                 $(this).html(dateDisplay);
            }
        });
    });
</script>
</body>
</html>
