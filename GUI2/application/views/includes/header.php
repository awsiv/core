<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<noscript>
  <META HTTP-EQUIV="Refresh" CONTENT="0;URL=<?php echo base_url()?>/disabled.html">
</noscript>
<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=9" />
<title><?php echo $title ;?></title>
<?php echo $this->carabiner->display('CFEngine'); ?>
<!--[if IE]><?php echo $this->carabiner->display('iefix'); ?><![endif]-->
<?php
$injected= isset($injected_item)?$injected_item:"" ;echo $injected;
$this->carabiner->display(); ?>
<?php
/**
 * FIX for back button click in OPera and safari for refreshing the bread crumbs in session list 
 */
if($this->agent->is_browser('Opera'))
{
?>
<script type="text/javascript">
 if (window.opera)
{
    window.opera.setOverrideHistoryNavigationMode('compatible');
    history.navigationMode = 'compatible';
}
    $(document).ready(function(){
        var path=window.location.pathname,
             newpath=path.replace(/\/index\.php\//,'');
             $('#breadcrumb').load('/widget/getbreadcrumbs',{url:newpath},function(){
             });
   });
</script>
<?
  }
if($this->agent->is_browser('Safari')){
?>
<script type="text/javascript">
   $ (window).bind("unload", function() { });
</script>
<?php }?>  
</head>

<body>
<!--[if lte IE 7]><script src="<?php echo get_scriptdir() ?>/ieupgrade/warning.js"></script><script>window.onload=function(){e("<?php echo get_scriptdir() ?>/ieupgrade/")}</script><![endif]-->
  <div id="wrapper" class="container_12">
          <div id="header">
              <div id="header_left"class="grid_8">
               <p class="sitetitle">
                  <?php echo anchor("welcome/index",'Nova mission portal') ?> 
                  <span class="version"></span>
               </p>
               <p class="breadcrumbs">
                  <ul id="breadcrumb">
                  <?php if(isset($breadcrumbs)){ echo $breadcrumbs;}?>
                      <p class="clearleft"></p>
                 </ul>
               </p>
               </div>

              <div id="header_right"class="grid_4">
                       <p id="userbox">
	                Hello  <?php echo $this->session->userdata('username');?>
                        &nbsp;| &nbsp;
                        <a href="<?php echo site_url('auth/logout'); ?>" title="logout">logout</a>
                        <span class="online_users"> Online users: <strong id="ttlonlinenum"><?php echo $this->onlineusers->total_users()?></strong></span>
                       </p>
                      <p class="clearright"></p>
                      <div id="webadmin">

                          <div id="searcharea">
                          <form action="<?php echo site_url('knowledge/knowledgeSearch')?>" method="post">
                              <label id="searchbox">
                                  <input type="text" name="search" placeholder="Search in knowledge map" value="<?php echo  !isset($search) ? "":$search ?>" />
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