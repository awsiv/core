<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Login</title>
<style>
body {
background: #EEE;
color: #333;
font-family: Helvetica, Arial, sans-serif;
font-size: 12px;
line-height: 16px;
text-shadow: white 0px 1px 0px;
}

h1.logo{
	display: block;
	color: #444;
font-size: 50px;
font-weight: bold;
margin: 60px auto 0px;
padding-left: 15px;
text-align: center;
text-decoration: none;
text-shadow: #CCC 1px 2px 5px;
}

h1.logo .grey{
	color: #BBB;
font-style: normal;
text-shadow: #777 1px 2px 2px;
}

#login h3 {
border-bottom: 1px solid #CCC;
font-size: 14px;
font-weight: bold;
letter-spacing: 2px;
margin-bottom: 20px;
text-transform: uppercase;
}

#login {
-webkit-box-shadow: #AAA 0px 0px 15px;
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(white), to(#CCC));
border: 1px solid #AAA;
border-bottom-left-radius: 10px 10px;
border-bottom-right-radius: 10px 10px;
border-top-left-radius: 10px 10px;
border-top-right-radius: 10px 10px;
-moz-border-radius: 10px 10px 10px 10px;
-moz-box-shadow: 0 0 15px #AAAAAA;
background: -moz-linear-gradient(90deg, #CCCCCC, #FFFFFF) repeat scroll 0 0 transparent;
background: -o-linear-gradient(90deg, #CCCCCC, #FFFFFF) repeat scroll 0 0 transparent;
margin: 40px auto 0px;
padding: 20px;
width: 311px;
}

#login #start {
display: block;
}

#credits {
font-size: 9px;
margin-top: 20px;
text-align: center;
}

label {
color: #555;
cursor: pointer;
display: block;
float: left;
font-size: 13px;
font-weight: bold;
line-height: 28px;
margin-bottom: 5px;
width: 120px;
}

input[type="text"], input[type="password"], select {
-webkit-box-shadow: #AAA 0px 0px 3px;
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#EEE), color-stop(0.2, white), to(white));
border: 1px solid #AAA;
border-bottom-left-radius: 3px 3px;
border-bottom-right-radius: 3px 3px;
border-top-left-radius: 3px 3px;
border-top-right-radius: 3px 3px;
color: #666;
padding: 5px;
}

input.error {
border-color: #A60303;
margin: 0px;
}

input[type="submit"], a.submit {
-moz-border-radius: 3px 3px 3px 3px;
-moz-box-shadow: 0 0 3px #666666;
background: -moz-linear-gradient(90deg, #AAAAAA, #EEEEEE) repeat scroll 0 0 transparent;
-webkit-box-shadow: #666 0px 0px 3px;
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#EEE), to(#BBB));
background: -o-linear-gradient(90deg, #EEE #FFFFFF) repeat scroll 0 0 transparent;
border: 1px solid #555;
border-bottom-left-radius: 3px 3px;
border-bottom-right-radius: 3px 3px;
border-top-left-radius: 3px 3px;
border-top-right-radius: 3px 3px;
color: #444;
cursor: pointer;
font-family: Arial, sans-serif;
font-size: 12px;
font-weight: bold;
padding: 5px 10px;
text-decoration: none;
text-shadow: white 0px 1px 1px;
text-transform: uppercase;
}


input[type="submit"]:hover, a.submit:hover {
background: -webkit-gradient(linear, 0% 0%, 0% 100%, from(#EEE), to(#CCC));
background: -o-linear-gradient(90deg, #EEE, #FFFFFF) repeat scroll 0 0 transparent;
border-color: #333;
text-decoration: none;
}

.error{
color: #D8000C;
background-color: #FFBABA;
padding:5px 10px;
border:solid 1px;
text-shadow:none;
}
</style>
</head>

<body>
<h1 class="logo">
 CFEngine
  <span class="grey">Nova</span>
</h1>
<div id="login" class="grey">
    <script type="text/javascript">
   /* $(document).ready(function(){
     $("#start").validate({
        rules: {
            email: {
                required: true,
                email: true
                },
            password: {
                required: true
            }
            }
        });
    });*/
    </script>

    <?php echo $message; ?>
            
     <?php echo form_open("auth/login");?>
            <input type="hidden" id="type_form" name="type_form" value="login">
                <h3>Login</h3>
                        <p>
            <p>
          <label for="<?php echo strtolower($lbl_identifier);?>"><?php echo $lbl_identifier;?></label>
          <?php echo form_input($identifier);?>
           </p>
            </p>
            <p>
                  <label for="password">Password:</label>
      	         <?php echo form_input($password);?>
            </p>
             <p>
              <label for="remember">Remember Me:</label>
              <?php echo form_checkbox('remember', '1', FALSE);?>
	        </p>
            <p>
              <?php echo form_submit('submit', 'Login');?>
            </p>
     <?php echo form_close();?>
      
        
</div>
<p id="credits">
    <strong>Configuration from within</strong><br />
     © 2011 - <a href="http://cfengine.com">CFEngine</a>
</p>

</body>
</html>