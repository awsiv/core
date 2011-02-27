<?php 
#
# This file is (C) Cfengine AS. All rights reserved
#
//cfpr_header("overview","normal");  
//cfpr_menu("Home : overview");
$ret1 = cfpr_getlicense_owner();
$all = cfpr_count_all_hosts();
$r = cfpr_count_red_hosts();
$y = cfpr_count_yellow_hosts();
$g = cfpr_count_green_hosts(); 
?>
<div id="dashboard" class="ui-widget-wrapper ui-widget-content grid_10 push_1">
    <div class="titleborder">
        <h1>Quick tasks</h1>
    </div>
    <ul id="dashboard-buttons">
        <li><?php echo anchor('welcome/status','Over all',array('title'=>'Summarized status of all host','class'=>'overall'))?></li>
        <li><?php echo anchor('welcome/listhost','Individual',array('title'=>'Status of particular host','class'=>'individual'))?></li>
        <li><?php echo anchor('welcome/pulse_vitals','Vitals',array('title'=>'Vital signs of particular host','class'=>'vitals'))?></li>
        <li><?php echo anchor('auth/Index','Administration',array('title'=>'Administer the content and users','class'=>'admin'))?></li>
        <li><?php echo anchor('welcome/helm','Configuration',array('title'=>'Work on Policies to configure system','class'=>'configure'))?></li>
        <li><?php echo anchor('welcome/knowledge','Knowlegde',array('title'=>'Knowledge map of your system','class'=>'knowledge'))?></li>
    </ul>
    <!--<div id="reports" class="grid_4 push_2 panelcontent">
        <ul>
            <li><?php echo anchor('welcome/status','All Hosts','title=allhosts')?></li>
            <li><?php echo anchor('welcome/listhost','Host Only','particular host')?></li>
            <li><?php echo anchor('welcome/hubs','Hubs','all hubs')?></li>
            <li><?php echo anchor('welcome/pulse_Vitals','View Pulse and Vitals','pulse and vitals')?></li>
        </ul>
    </div>
    <div id="planning" class="grid_4 push_2 panelcontent">
        <ul>
            <li><?php echo anchor('welcome/services','Enterprise Goals','title=Goals')?></li>
            <li><?php echo anchor('welcome/cfeditor','Develop Policy','title=work on policy')?></li>
            <li><?php echo anchor('welcome/search/index/report/Business value report/manyhosts/1','Latest enterprise cost and Value','Cost and Value')?></li>
        </ul>
    </div>
    <div class="clear"></div>
    <div id="Administration" class="grid_4 push_2 panel panelcontent">
        <ul>
            <li><?php echo anchor('auth/Index','Users','title=manage users')?></li>
            <li><?php echo anchor('auth/manage_group','Group','title=manage group')?></li>
        </ul>
    </div>
    <div id="KnowlegdeMap" class="grid_4 push_2 panelcontent">
       <ul>
            <li><?php echo anchor('welcome/knowledge','View KnowldegeMap','title=Knowledge')?></li>
            <li><?php echo anchor('welcome/knowledge/topic/security view','Security','Security')?></li
            <li><?php echo anchor('welcome/Knowledge/topic/policy.*','Policy guidance','policy guidance')?></li>
            <li><?php echo anchor('welcome/Knowledge/topic/best.*practice','Policy guidance','policy guidance')?></li>
       </ul>
    </div>
    <div class="clear"></div>-->
</div>

          <div class="grid_4">
                  <div class="panel">
                    <div class="panelhead"><?php echo $all?> hosts registered</div>
                     <ul class="panelcontent">
<li><a href="<?php echo site_url()?>/welcome/hosts/red"><img src="<?php echo get_imagedir();?>red_sign_medium.png" class="align"/><span class="imglabel"><?php echo $r?> hosts known</span></a></li>
<li><a href="<?php echo site_url()?>/welcome/hosts/yellow"><img src="<?php echo get_imagedir();?>yellow_sign_medium.png" class="align"/><span class="imglabel"><?php echo $y?> hosts known</span></a></li>
 <li><a href="<?php echo site_url()?>/welcome/hosts/green"><img src="<?php echo get_imagedir();?>green_sign_medium.png" class="align"/><span class="imglabel"><?php echo $g?> hosts known</span></a></li>
     <li class="note"><a href="license"><img src="<?php echo get_imagedir();?>info.png" class="align"/><span class="imglabel">This edition is licenced to <?php echo $ret1?></span></a></li>
                     </ul>
                     <p>
                     
                     </p>
                  </div>
           </div>
        <?php cfpr_compliance_summary_graph();?>  
          <div class="grid_8">
           	<div class="panel">
          		<div class="panelhead">Compliance summary</div>
                <div class="panelcontent">
                  <img src="<?php echo get_hubdir();?>common/compliance.png" />
                  <ul class=" grid_2 grp_label">
                  	<li><img src="<?php echo get_imagedir();?>button-green.png" class="align"/><span class="imglabel">kept</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-yellow.png" class="align"/><span class="imglabel">repaired</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-red.png" class="align"/><span class="imglabel">not kept</span></li>
                    <li><img src="<?php echo get_imagedir();?>button-orange.png" class="align"/><span class="imglabel">no data</span></li> 
                  </ul>
                  <div class="clear"></div>
                 </div>
          	</div>
          </div>
          
          <div class="clear"></div>
          <script type="text/javascript">
              $(document).ready(function()
              {
                $('#dashboard-buttons').find('a[title]').qtip({
                 style: {
                     width:200,
                     background:'#454545',
                     name: 'dark',
                      border: {
                                  width: 1,
                                  radius: 1
                              },
                     tip:{
                         corner:'topMiddle',
                         size:{
                             x:10,
                             y:10
                             }
                     }
                 },
                 show:{
                     effect:{
                        type:'fade',
                        lenght:200
                     }
                 },

                 position: {
                     corner: {
                             target: 'bottomMiddle',
                             tooltip: 'topMiddle'
                          }
                       }
                  });
              });

          </script>


