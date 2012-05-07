
    <div class="outerdiv grid_12">
        <div id="goals" class="grid_7 innerdiv">
            <p class="title"><?php echo $this->lang->line('header_policy_goals'); ?></p>
            <?php require_once('goals-template.php'); ?>
        </div>
        <div id="loggedon" class="grid_5 innerdiv">
            <p class="title"><?php echo $this->lang->line('header_logged_on'); ?></p>
            <ul id="users">
                <?php
               $totalusersonline=count($users);
                foreach ($users as $user) {
                    echo '<li>' . $user . ': ' . $this->userdata->get_personal_working_log_latest($user) . '</li>';
                }
                ?>
            </ul>
            <?php if($totalusersonline >5){?>
            <p class="morebtnpane"><span class="morebtn"><a id="moreusers" href="#">More...</a></span></p>
            <?php } ?>
        </div>
        <div class="clearboth"></div>

        <div id="plandash" class="grid_7 dash innerdiv">
            <ul>
                <li><?php echo anchor('/repository/checkout', $this->lang->line('edit_policies') , array('class' => 'repolink showqtip','id' => 'repobrowser','title'=> tooltip('tooltip_edit_policies'))); ?></li>
                <li><?php echo anchor('/widget/tracker', $this->lang->line('track_record'), array('class' => 'trackrecord showqtip', 'id' => 'trackrecord','title'=> tooltip('tooltip_track_records'))); ?></li>
                <li><?php echo anchor('/repository/policyApprover', $this->lang->line('approved_policies'), array('class' => 'approve showqtip', 'id' => 'aprvpolicy','title'=> tooltip('tooltip_approve_policies'))); ?></li>
                 <li><?php echo anchor('/welcome/services', $this->lang->line('service catalogue'), array('class' => 'service showqtip ', 'id' => 'aprvpolicy','title'=> tooltip('tooltip_service_catalogue'))); ?></li>
                <p class="clearleft"></p>
            </ul>
        </div>
        <div id="personallog" class="grid_5 innerdiv">
            <p class="title"><?php echo $this->lang->line('header_activity_log'); ?></p>
            <div id="messageinput_container">
                <form method="post" action="<?php echo site_url(); ?>/widget/insertworkinglogs">
                    <textarea  id="message" name="message" placeholder="Working on...."></textarea>
                    <div id="button_block">
                        <input type="submit" id="button" value=" Share" class="slvbutton"/>
                        <input type="button" id='cancel' value=" cancel" />
                    </div>
                </form>
            </div>
            <ul id="personalnotes">
                <?php
                foreach ($working_notes as $note) {
                    echo "<li>" . $note['working_on'] . "<span class=\"note_date\">" . date('D F d h:i:s Y', $note['date']) . "</span> </li>";
                }
                ?>
            </ul>
            <p class="morebtnpane"><span class="morebtn"><?php echo anchor('welcome/workingNotes', 'More...') ?></span></p>
        </div>
        <div class="clearboth"></div>
    </div>
<div class="clear"></div>
<!--<script src="<?php echo get_nodehost()?>/socket.io/socket.io.js" type="text/javascript"></script>-->
<script type="text/javascript">
    $(document).ready(function(){
        /* var socket = new io.Socket(null, {port: 8080});
            socket.connect();
            socket.on('message', function(msg) {
                   $('#users').load('/widget/getworkinglatestlogs')
                   // console.log(msg);
          });*/
      
        $("#message").focus(function()
        {
            $(this).animate({"height": "85px"}, "fast" );
            $("#button_block").slideDown("fast");
            return false;
        });

        $("#cancel").click(function()
        {
            $("#message").val('').animate({"height": "30px"}, "fast" );
            $("#button_block").slideUp("fast");
            //socket.send("reload");
            return false;
        });

        $("#personalnotes").find('li').hover(
        function () {
            $(this).find('span:last').addClass('display')
        },
        function () {
            $(this).find('span:last').removeClass('display')
        }
    );

        $('#messageinput_container').find('form').submit(function(e){
            e.preventDefault();
            // return false;
            $.ajax({
                type: "POST",
                async:false,
                url: $(this).attr('action'),
                data:$(this).serialize(),
                success: function(data){
                    $('#personalnotes').html(data);
                    $('#users').load('<?php echo site_url(); ?>/widget/getworkinglatestlogs')
                    $("#message").val('').animate({"height": "30px"}, "fast" );
                    $("#button_block").slideUp("fast");
                    // socket.send("reload");
                },
                error:function(data){
	            	
                }
            });
        });
       
        $('#repobrowser').ajaxyDialog({title:'SVN Checkout',dontOverrideTitle:true});
        $('#aprvpolicy').ajaxyDialog({title:'Approve Policies',dontOverrideTitle:true});
        $('#trackrecord').ajaxyDialog({
            title:'Track Record',
            dontOverrideTitle:true,
           width:'1051',
           height:'600'
        });
     
$('#users').css('max-height', '190px')
        //$("#users").height('190px');
        $('#moreusers').click(function(){
        $("#users").css('height', 'auto'). css('max-height','none');
        });
    });
</script>
