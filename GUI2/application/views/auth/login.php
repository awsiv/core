<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>CFEngine Mission Portal - Login</title>
<link media="screen" href="<?php echo get_cssdir().'appstyle.css';?>" rel="stylesheet" type="text/css"></link>
</head>

<body>
    <!--[if lte IE 7]><script src="<?php echo get_scriptdir() ?>/ieupgrade/warning.js"></script><script>window.onload=function(){e("<?php echo get_scriptdir() ?>/ieupgrade/")}</script><![endif]-->
    <div id="login">
        <h1 class="logo"></h1>
        <div class="inner">
               <?php 
              echo $message;
                 ?>
                <?php echo form_open("auth/login");?>
                <input type="hidden" id="type_form" name="type_form" value="login"></input>
                    <h3>Login <span class="modeinfo">(<?php echo $mode?>)</span></h3>
                     <hr></hr>
                    <p>
                      <label for="<?php echo strtolower($lbl_identifier);?>"><?php echo $lbl_identifier;?></label>
                      <?php echo form_input($identifier);?>
                   </p>
                    <p>
                     <label for="password">Password</label>
      	         <?php echo form_input($password);?>
                   </p>
                    <p>
                       <span class="agentlogo"></span>
                           <?php echo form_submit('submit', '');?>
                           <label for="remember" class="remember">Remember me</label>
                            <?php echo form_checkbox('remember', '1', FALSE);?>
                    </p>
                    
                <?php echo form_close();?>
        </div>
    </div>
     <p id="credits">
      <strong>Precision in IT Infrastructure Engineering</strong><br />
     © 2011 - <a href="http://cfengine.com">CFEngine</a>
    </p>
</body>
</html>
