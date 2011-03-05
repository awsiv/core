<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title><?php echo $title ;?></title>
<link href="<?php echo get_cssdir();?>resets.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>960.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>style.css" rel="stylesheet" media="screen" />
<script src="<?php echo get_scriptdir();?>jquery-1.4.2.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery.tablesorter.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>picnet.jquery.tablefilter.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery.tablesorter.pager.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery.qtip-1.0.min.js" type="text/javascript"></script>
<?php $injected= isset($injected_item)?$injected_item:"" ;echo $injected;?>
</head>

<body>
<div id="wrapper">
   <div class="container_12">
    	<div id="header">
            <div class="grid_9 sitetitle">
                <a href="<?php echo site_url('welcome/index'); ?>" title="dashboard">
                CFENGINE MISSION PORTAL
                <span id="subheading"> <b><?php echo $title_header ;?></b></span>
                </a>
            </div>
            <p id="userbox" class="grid_2">
	      Hello! <strong><?php echo $this->session->userdata('username');?></strong>
               &nbsp;| &nbsp;
             <a href="<?php echo site_url('auth/logout'); ?>" title="logout">logout</a>
            </p>
            <div class="clear"></div>
        </div>