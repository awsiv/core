<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title><?php echo $title ;?></title>
<link href="<?php echo get_cssdir();?>resets.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>960.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>Base.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>appstyle.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
<script src="<?php echo get_scriptdir();?>jquery-1.4.2.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>
<!--[if IE]><?php echo $this->carabiner->display('iefix'); ?><![endif]-->
<?php
$injected= isset($injected_item)?$injected_item:"" ;echo $injected;
$this->carabiner->display(); ?>
</head>

<body>
  <div id="wrapper" class="container_12">
          <div id="header">
              <div id="header_left"class="grid_7">
               <p class="sitetitle"><?php echo anchor("welcome/index",'cfengine mission portal') ?></p>
               <p class="breadcrumbs">
                  <ul id="breadcrumb">
                  <?php if(isset($breadcrumbs)){ echo $breadcrumbs;}?>
                      <p class="clearleft"></p>
                 </ul>
               </p>
               </div>

              <div id="header_right"class="grid_5">
                       <p id="userbox">
	                Hello  <strong><?php echo $this->session->userdata('username');?></strong>
                        &nbsp;| &nbsp;
                        <a href="<?php echo site_url('auth/logout'); ?>" title="logout">logout</a>
                       </p>
                      <p class="clearright"></p>
                      <div id="webadmin">

                          <div id="searcharea">
                          <span class="online_users"> Online users: <strong><?php echo $this->onlineusers->total_users()?></strong></span>
                          <form action="<?php echo site_url('knowledge/knowledgeSearch')?>" method="post">
                              <label id="searchbox">
                                  <input type="text" name="search" placeholder="Search in knowledge map" value="<?php echo  !isset($search) ? "Search in knowledge map":$search ?>" />
                              </label>
                          </form>
                          </div>

                             <?php echo anchor('auth/admin_page',' ',array('class'=>'adminbtn'));?>
                             <?php echo anchor('auth/setting',' ',array('class'=>'settingbtn'));?>

                      </div>
                      <p class="clearright"></p>

             </div>
             <div class="clear"></div>
          </div><!--end of header-->