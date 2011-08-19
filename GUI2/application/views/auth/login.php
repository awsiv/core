<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Login</title>
<style>
    @font-face {
    font-family: "CFE_FONT";
    font-style: normal;
    font-weight: normal;
    src: local('☺'), url("../fonts/ttf/opensans-regular-webfont.ttf") format("truetype"), url("../fonts/svg/opensans-regular-webfont.svg") format("svg");
}
html {
  height:100%  
}
body {
/*background-repeat: no-repeat;
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#F8F8F5), to(#D7D8DA));
background: -moz-linear-gradient(90deg, #D7D8DA, #F8F8F5) ;
background: -o-linear-gradient(90deg, #D7D8DA, #F8F8F5);*/
background: url("../../themes/default/images/bg_tile.png") repeat-x scroll 0 0 #A5ACA2;
color: #333;
font:13px/21px 'CFE_FONT',arial,Helvetica,sans-serif;
}

h1.logo{
	background: url('../../themes/default/images/cf_heading.png') no-repeat;
                width:214px;
                height: 31px;
                margin: 25px 0 21px;
}

#login {
border-bottom-left-radius: 6px 6px;
border-bottom-right-radius: 6px 6px;
border-top-left-radius: 6px 6px;
border-top-right-radius: 6px 6px;
background:#383838;
margin: 65px auto 0;
padding: 35px;
width: 376px;
}

#login h3 {
font-size: 17px;
font-weight: normal;
margin-bottom: 9px;
text-transform: uppercase;
color:#383838;
margin:0;
}

#login hr{
    border: solid 1px #fff;
    border-bottom: 23px;
}

#login .inner{
    background-color: #EBEBEB;
    border-bottom-left-radius: 6px 6px;
    border-bottom-right-radius: 6px 6px;
    border-top-left-radius: 6px 6px;
    border-top-right-radius: 6px 6px;
    padding:40px;  
}

#login .inner p
{
    margin:23px 0 0 0;
}

#login .agentlogo{
    background: url('../../themes/default/images/cf_agent.png') no-repeat transparent center center;
    width:120px;
    height: 44px;
    display:block;
    float:left;
}

#login .remember{
    font-size: 12px;
    width:100px;
}

#login #start {
display: block;
}

#credits {
font-size: 9px;
margin-top: 21px;
text-align: center;
}

#credits a{
    color:#333
}

label {
color:#383838;
cursor: pointer;
display: block;
float: left;
font-size: 17px;
font-weight: normal;
line-height: 28px;
margin-bottom: 5px;
width: 120px;
}

input[type="checkbox"]{
   vertical-align: bottom;
   margin-top: 7px;
}

input[type="text"], input[type="password"], select {
-webkit-box-shadow: #AAA 0px -1px 3px;
-moz-box-shadow:0 -1px 3px #AAA;
 box-shadow:0 -1px 3px #AAA;
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#EEE), color-stop(0.2, white), to(white));
border: 1px solid #AAA;
border-bottom-left-radius: 3px 3px;
border-bottom-right-radius: 3px 3px;
border-top-left-radius: 3px 3px;
border-top-right-radius: 3px 3px;
color: #666;
padding: 5px;
font-size: 14px;
width:156px;
}

input.error {
border-color: #A60303;
margin: 0px;
}

input[type="submit"], a.submit {
background: url('../../themes/default/images/login_button.png') no-repeat transparent center center;
border:none;
width: 61px;
height: 25px;
cursor: pointer;
display:block;
}


input[type="submit"]:hover, a.submit:hover {

}

#login p.error{
color: #D8000C;
background-color: #FFBABA;
padding:5px 10px;
border:solid 1px;
text-shadow:none;
margin:0 0 10px 0;
}

</style>
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
                    <h3>Login</h3>
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
                           <label for="remember" class="remember">Remember Me</label>
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
