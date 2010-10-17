<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Untitled Document</title>
<script src="scripts/jquery-1.4.2.min.js" type="text/javascript"></script>
<script src="scripts/jquery-ui-1.8.2.custom.min.js" type="text/javascript"></script>
<link href="css/jquery-ui-1.8.2.custom.css" rel="stylesheet" media="screen" />
</head>

<body>
<span><a href="#sendmail" id="send_mail">Send mail</a></span>
      <div title="Send mail" id="dialog" style="display:none">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="to_contacts">To:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="to_contacts" name="to_contacts" />
                    <label for="subject">Subject:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="subject" name="to_contacts" />
                    <textarea class="ui-widget-content ui-corner-all" id="mail_desc" name="mail_desc"></textarea>
                    <input type="hidden" id="event"></input>
                </fieldset>
            </form>
        </div>
</body>
<script type="text/javascript">
$(document).ready(function() {
$('a#send_mail').click(function(e){	
	  e.preventDefault();						
      $dialog.dialog('open');
	});

var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 buttons: {
		 'Send': function() {
		 $.ajax({
           type: "POST",
           url: "sendmail.php",
           data:({'to':$('#to_contacts').val(),'subject':$('#subject').val(),'content':$('#mail_desc').val()}),
           dataType:'json',
           success: function(data){
        	  
			  
             }
          });
		 $(this).dialog('close');
		 },
		 'Cancel': function() {
		 $(this).dialog('close');
		 }
		 },
		 open: function() {
		 $('#to_contacts').focus();
		 
		 },
		 close: function() {
		 
		 }
		 });
	});
</script>
</html>