<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#
      
$commentid = cfpr_get_host_commentid($hostkey);
$comments="";
if ($hostkey != "" && ($_POST["username"]) && ($_POST["commentArea"]) && ($op=="addcomment")) {
   cfpr_comment_add($hostkey,"",$commentid,1,"$hostname,$ipaddr",$_POST["username"],time(),$_POST["comment_text"]);
}

cfpr_host_meter($hostkey);
$colour = cfpr_get_host_colour($hostkey);
  ?>
        
         <div class="grid_5">
                  <div class="panel">
                    <div class="panelhead">Select host: <?php if($hostkey=="none") echo "No host selected" ;?> </div>
                     <div class="panelcontent">
                     <form method="post" action="<?php ?>">
                     <?php
                     $allhosts = cfpr_select_hosts($hostkey,".*",100);
                     echo "$allhosts";
					 ?>
                     <input class="btn floatRight"  type="submit"  value="Select host" />
                     <div class="clearright"></div>
                     </form>
                     </div>
                  </div>
              <?php
				$last = cfpr_getlastupdate($hostkey);
				$class = cfpr_get_variable($hostkey,"sys","ostype");
				$flavour = cfpr_get_variable($hostkey,"sys","flavour");
				$rel = cfpr_get_variable($hostkey,"sys","release");
				$load = cfpr_get_variable($hostkey,"mon","av_loadavg");
				$free = cfpr_get_variable($hostkey,"mon","av_diskfree");
				$speed = cfpr_get_network_rate($hostkey);
			  ?>
                  <div class="panel">
                     <div class="panelhead">Host Details (discovered)</div>
                     <div class="panelcontent">
       <p><label class="width_20">Alias:</label><label ><?php echo $hostname?></label></p>
       <p><label class="width_20">OS class:</label><label><?php echo $class?></label></p>
       <p><label class="width_20">Release:</label><label><?php echo $rel?></label></p>
       <p><label class="width_20">Flavour:</label><label><?php echo $flavour?></label></p>
       <p><label class="width_20">Last IP-address:</label><label><?php echo $ipaddr?></label></p>
       <p><label class="width_20">Last data:</label><label><?php echo $last?></label></p>
       <p><label class="width_20">ID:</label><label><small><?php echo $hostkey?></small></label></p>
                      </div>
                 </div>

                 <div class="panel">
                     <div class="panelhead">Comments</div>
                     <div class="panelcontent">
                     <p><label class="width_20">hostKey:</label><label ><?php echo $hostname?></label></p>
                     <?php
                        if ($commentid != "")
                        {
                         echo "<div> <a href=\"#\" class=\"show_comment_btn\">Show Comments ($commentid)</a> </div>";
                        }
                        else
                        {
                         echo "<div> <a href=\"#\" class=\"add_comment_btn\">Add Comment</a> </div>";
                        }
                     ?>

                     <div id="comments">
                       <?php
                       if ($commentid != "")
                       {
                         $comments = cfpr_comment_query('',$commentid,'',-1,-1);
                         echo $comments;
                         echo "<div> <a href=\"#\" class=\"add_comment_btn\">Add Comment</a> </div>";
                       }
                     ?>
                     </div>

                     <div id="add_comment">
                       <form name ="add_comment_form" action="host.php" method="post">
                        <p><label>Username: </label><input type="text" name="username"></p>
                        <p> <textarea name="comment_text" rows="10" cols="50"> </textarea> </p>
                        <input type="hidden" name="hostkey" id="hostkey" value="<?php echo $hostkey?>"/>
                        <input type="hidden" name="op" id="op" value="addcomment"/>
                        <input type="submit" value="Comment!" name="submit_comment">
                        </form>
                     </div>

                 <div class="panel">
                   <div class="panelhead">Status (measured)</div>
                     <div class="panelcontent">
               <div class="width_80 floatLeft">
               <p><label class="width_20">Average Load:</label><label ><?php echo $load?>%</label></p>
               <p> <label class="width_20">Average Free Disk:</label><label><?php echo $free?>%</label></p>
               <p><label class="width_20">Average network speed:</label><label><?php echo  $speed?></label></p>
               </div>
               <div class="floatRight">
               <img src="<?php echo get_imagedir().$colour."_sign_medium.png" ?>" class="floatRight"/>
                <form method="post" action="host.php" onsubmit="return confirm('The host will be deleted from the report database of the hub. The hub will still try to pull it for reports, and it may thus reappear. Are you sure you want to delete this host?');">
                     <input type="hidden" name="op" id="op" value="delete"/>
                     <input type="hidden" name="delhost" id="delhost" value="<?php echo $hostkey?>"/>
                     <input class="btn margin-top10"  type="submit"  value="Delete host"/>
                 </form>
                </div>
               <div class="clearboth"></div>
                     </div>
                  </div>
             </div>
          <?php cfpr_summary_meter(NULL);?>
          <div class="grid_7">
           	<div class="panel">
          		<div class="panelhead">Analysis</div>
                <div class="panelcontent">
                <p><a href="knowledge.php?topic=Status level meters"><img src="<?php echo get_hubdir().$hostkey?>/meter.png"></a></p>
                <p><a href="<?php echo site_url('visual/vital').'/'.$hostkey ?>" ><img src="<?php echo get_imagedir()?>pulsed.png" class="align"/></a></p>
                 </div>
          	</div>
            
            <div class="panel">
          		<div class="panelhead">Generate report</div>
                <div class="panelcontent">
                <form method="post" action="<?php echo site_url('search/')?>">
                <p>Simple search string:</p>
                <p><input class="searchfield" type="text" name="search" /></p>
                <p><?php $allreps = cfpr_select_reports(".*",100);
				  echo "$allreps";?></p>
                <input type="hidden" name="hostkey" value="<?php echo $hostkey?>">
                <p><input type="submit" value="commit"></p>
                </form>
                </div>
             </div>
             
              <div class="panel">
          		<div class="panelhead">Monitored jobs</div>
                <div class="panelcontent">
                <?php
                $number = cfpr_get_class_frequency(NULL,"mXC.*");
                $nlist = cfpr_report_class_frequency($hostkey,"mXC.*");
                ?>
                <p>Total number under surveillance:<?php echo $number?><p>
                <p>On this host: <?php echo $nlist?></p> <!--proble, with varaibale nlist-->
                </div>
              </div>
          </div>
          <div class="clear"></div>
          
          
			<div class="panel">
			<?php $report = cfpr_summarize_notkept($hostkey,NULL,NULL);?>
			<div class="panelhead">Promises not kept in the past week</div>
                <div class="panelcontent">
                      <div class="tables">
                      <?php echo $report ?>
                      </div>
                </div>
            </div>
 
<script type="text/javascript">
$(document).ready(function() { 
    $('.tables table').prepend(
        $('<thead></thead>').append($('.tables tr:first').remove())
        );

    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']}); 
	var oldlink=$("#tabpane table a").attr('href');
	var newlink=oldlink.split('?');
	var opt=newlink[1].split('=');
	$("#tabpane table a").attr('href','<?php echo site_url()?>/welcome/promise/'+opt[1]);
	//the link generated shoud be site_url/welcome/promise/param1/param2
	
});
</script>

<script type="text/javascript" src="scripts/jquery-1.4.2.min.js"></script>
 <script type="text/javascript">
$('#comments').hide();
$('#add_comment').hide();

$('.show_comment_btn').click(function() {
    $('#comments').slideToggle('slow');
    $(this).text($(this).text() == 'Hide Comments' ? 'Show Comments' : 'Hide  Comments');
    return false;
});

$('.add_comment_btn').click(function() {
    $('#add_comment').slideToggle('slow');
   $(this).text($(this).text() == 'Hide Comments' ? 'Show Comments' : 'Hide  Comments');
    return false;
});

 </script>
