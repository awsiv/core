
<div id="footer">
    <p class="grid_7"><?php echo cfpr_enterprise_version(); ?> running on <?php echo cfpr_community_version(); ?> </p>
    <p class="grid_5 alignright">Copyright &copy; 2011 CFEngine AS - All rights reserved</p>
    <p class="clear"></p>
</div>
</div>
</body>
</html>
<script type="text/javascript">
    // q tip initialization 
    $(document).ready(function() {
        
        
        $('a.showqtip,a.help').live('mouseover', function(event) {
            // Bind the qTip within the event handler
            $(this).qtip({
                overwrite: false, // Make sure the tooltip won't be overridden once created
                position: {
                    at:'Bottom Middle'
                },
                show: {
                    event: event.type, // Use the same show event as the one that triggered the event handler
                    ready: true // Show the tooltip as soon as it's bound, vital so it shows up the first time you hover!
                }
            }, event); // Pass through our original event to qTip
        })
       
    });
</script>