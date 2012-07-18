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
<?php
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

      <?php if (!isset ($hide_header) || $hide_header !== true) { ?>
        <div id="wrapper" class="container_12">
          <div id="header">
              <div id="header_left" class="grid_8">
               <p class="sitetitle">
                  <?php
                  $text=is_constellation()?$this->lang->line('cons_logo_link_text'):$this->lang->line('nova_logo_link_text');
                  echo anchor("welcome/index", $text ) ?>
                  <span class="version"></span>
               </p>
               <p class="breadcrumbs">
                  <ul id="breadcrumb">
                  <?php if(isset($breadcrumbs)){ echo $breadcrumbs;}?>
                      <p class="clearleft"></p>
                 </ul>
               </p>
               </div>

              <div id="header_right" class="grid_4">
                       <p id="userbox">
	                Hello  <?php echo $this->session->userdata('username');?>
                        &nbsp;| &nbsp;
                        <a href="<?php echo site_url('login/logout'); ?>" title="logout">logout</a>
                        <span class="online_users"> Online users: <strong id="ttlonlinenum"><?php echo $this->onlineusers->total_users()?></strong></span>
                        <br /><span class="online_users">you are logged in with <?php
                        switch ($this->session->userdata('mode')) {
                            case 'ldap':             echo 'LDAP authentication'; break;
                            case 'active_directory': echo 'Active directory authentication'; break;
                            default:                 echo 'Internal authentication';
                        }
                        ?></span>
                       </p>
                      <p class="clearright"></p>
                      <div id="webadmin">

                          <div id="searcharea" class="searchbox">
                              <form action="<?php echo site_url('knowledge/knowledgeSearch')?>" method="post">
                                      <input type="text" name="search" placeholder="Search in knowledge map" value="<?php echo  !isset($search) ? "":$search ?>" />
                                      <a href="#" class="searchsubmit">&nbsp;</a>
                              </form>
                          </div>
                             <?php //if($this->ion_auth->mode == "internal") { echo anchor('auth/admin_page',' ',array('class'=>'adminbtn'));}
                             if ($this->ion_auth->is_admin() == true ) {
                                echo anchor('auth/admin_page', ' ', array('class'=>'btn adminbtn'));
                             }
                             else {
                                echo anchor('auth/view_profile', ' ', array('class'=>'btn adminbtn'));
                             }
                              echo anchor('settings/index',' ',array('class'=>'btn settingbtn'));?>
                      </div>
                      <p class="clearright"></p>

             </div>
             <div class="clear"></div>
          </div><!--end of header-->
      <?php } else { ?>
          <div id="wrapper" class="container_12 no_website_header">
      <?php } ?>