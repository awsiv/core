
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
        
        
        $("a.showqtip,a.help").live("mouseover", function(){

            $(this).qtip({
                show: { ready: true }, 
                position: {
                    my: 'top left',  // Position my top left...
                    at: 'bottom middle' // at the bottom right of...
                }

        
            });
        });
    });
       
</script>