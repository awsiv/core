<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title><?php echo $title ;?></title>
<link href="<?php echo get_cssdir();?>resets.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>960.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>style.css" rel="stylesheet" media="screen" />
<script src="<?php echo get_scriptdir();?>jquery-1.4.2.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery-ui-1.8.2.custom.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery.tablesorter.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>picnet.jquery.tablefilter.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery.tablesorter.pager.js" type="text/javascript"></script>
<?php $injected= isset($injected_item)?$injected_item:"" ;echo $injected;?>
</head>

<body>
<div id="wrapper">
   <div class="container_12">
    	<div id="header">
            <div class="grid_6 sitetitle">CFENGINE MISSION PORTAL <span id="subheading"><b><?php echo $title_header ;?></b></span></div> 
  <div id="searchbox" class="grid_4"><form action="knowledge" method="post"><input type="text" name="search" class="align"/><input type="submit" class="searchbtn" value=""/></form></div>
            <div id="historynav" class="grid_2 alignright">
                <a href="javascript:history.go(-1)"><img src="<?php echo get_imagedir();?>icon_arrow_back.png"/></a>
                <a href="<?php echo site_url()?>"><img src="<?php echo get_imagedir();?>/home.png"/></a>
                <a href="javascript:history.go(+1)"><img src="<?php echo get_imagedir();?>icon_arrow_forward.png"/></a>
            </div>
            <div class="clear"></div>
        </div>