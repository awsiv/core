<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title>Cfengine Nova Policy Editor</title>
		<link rel="stylesheet" type="text/css" href="<?php echo base_url();?>application/assets/css/jquery-ui-1.8.2.custom.css" />
		<link rel="stylesheet" type="text/css" href="<?php echo base_url();?>application/assets/css/style.css" />
		<script src="<?php echo base_url();?>application/assets/scripts/jquery-1.4.2.min.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery.layout.min-1.2.0.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery-ui-1.8.2.custom.min.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery.contextmenu.src.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/codemirror.js" type="text/javascript"></script>
        <script src="<?php echo base_url();?>application/assets/scripts/cf.js" type="text/javascript"></script>
		<style type="text/css">
      .completions {
        border: 1px solid black;
        z-index: 10;
        overflow: hidden;
      }
      .completions select {
        outline: none;
        padding: 0;
        margin: 0;
        border: 0;
        background-color: white;
        font-family: monospace;
        font-size: 80%;
      }
      .CodeMirror-wrapping {
        border-top: 1px solid #911;
        border-bottom: 1px solid #911;
        padding: 3px;
      }
      .CodeMirror-line-numbers {
        margin: 0;
        padding: 0;
        padding-top: 3px;
        margin-right: .4em;
        width: 2.2em;
        color: #aaa;
        text-align: right;
        padding-right: .3em;
        font-size: 10pt;
        font-family: monospace;
      }
    </style>
		
    </head>
	<body>
	<div id="wrapper">
	 <div id="header">
		 <div id="title">
		  <h2>Cfengine Nova</h2><h2 class="green">Policy Editor</h2>
		  
		  <div id="menu">
		  <a href="#" id="new">New</a> |
		  <a href="#" id="compile">Check syntax</a> |
		  <a href="#" id="save">Save</a> |
		  <a href="#">Publish</a> |
		  <a href="https://cfengine.com/otrs/customer.pl">Open support ticket </a> |
		  <a href="mailto:contact@cfengine.com">Feedback</a>
		   </div> 
		   
		  <div id="search">
		   <input type="text" id="searchbox"></input>
		   <div id="searchlink"><a href="#">search</a></div>
		  </div>
		  <br class="clear" />
		
	  
	  </div>
	 </div>
	 <div id="content">
	 <?php echo $content ?>
	 </div>
	 <div id="footer">
	 </div>
	</div>
	</body>
</html>
