<div id="body" class="grid_12">
    <div class="outerdiv">
        <div id="goals" class="grid_7 innerdiv">
            <p class="title">Goals </p>
          <ul>
              <?php
                        foreach ($goals as $goal) {
                            $words = explode("_", $goal->name);
                            echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>$goal->desc</span><span class=\"check\"></span></li>";
                        }
                 ?>
          </ul>
           <p class="morebtnpane"><span class="morebtn"><?php echo anchor('welcome/services','More...')?></span></p>
        </div>
        <div id="loggedon" class="grid_5 innerdiv">
            <p class="title">Logged on </p>
            <ul id="users">
                <?php
                foreach ($users as $user) {
                    echo '<li>' . $user . ' : ' . $this->userdata->get_personal_working_log_latest($user) . '</li>';
                }
                ?>
            </ul>
            <p class="morebtnpane"><span class="morebtn">More...</span></p>
        </div>
        <div class="clearboth"></div>

        <div id="plandash" class="grid_7 dash innerdiv">
            <ul>
                <li><?php echo anchor('/repository/checkout', 'repository', array('class' => 'repolink', 'id' => 'repobrowser')); ?></li>
                <li><?php echo anchor('widget/tracker', 'track records', array('class' => 'trackrecord', 'id' => 'trackrecord')); ?></li>
                <li><?php echo anchor('/repository/policyApprover', 'approve policies', array('class' => 'approve ', 'id' => 'aprvpolicy')); ?></li>
                <p class="clearleft"></p>
            </ul>
        </div>
        <div id="personallog" class="grid_5 innerdiv">
            <p class="title">Personal log</p>
            <div id="messageinput_container">
                <form method="post" action="/widget/insertworkinglogs">
                    <textarea  id="message" name="message" placeholder="Working on...."></textarea>
                    <div id="button_block">
                        <input type="submit" id="button" value=" Share "/>
                        <input type="button" id='cancel' value=" cancel" />
                    </div>
                </form>
            </div>
            <ul id="personalnotes">
                <?php
                foreach ($working_notes as $note) {
                    echo "<li><span class=\"wrknoteslblorg\">Me:</span> " . $note['working_on'] . "<span class=\"note_date\">" . date('D F d h:i:s Y', $note['date']) . "</span> </li>";
                }
                ?>
            </ul>
            <p class="morebtnpane"><span class="morebtn"><?php echo anchor('welcome/workingNotes', 'More...') ?></span></p>
        </div>
        <div class="clearboth"></div>
    </div>
</div>
<div class="clear"></div>
<script type="text/javascript">
    $(document).ready(function(){
        $("#message").focus(function()
        {
            $(this).animate({"height": "85px",}, "fast" );
            $("#button_block").slideDown("fast");
            return false;
        });

        $("#cancel").click(function()
        {
            $("#message").val('').animate({"height": "30px",}, "fast" );
            $("#button_block").slideUp("fast");
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
                    $('#users').load('/widget/getworkinglatestlogs')
                    $("#message").val('').animate({"height": "30px",}, "fast" );
                    $("#button_block").slideUp("fast");
                },
                error:function(data){
	            	
                }
            });
        });
       
        $('#repobrowser').ajaxyDialog({title:'SVN Checkout'});
        $('#aprvpolicy').ajaxyDialog({title:'Approve Policices'});
        $('#trackrecord').ajaxyDialog({
            title:'Track Record'
        });
          
    });
</script>
