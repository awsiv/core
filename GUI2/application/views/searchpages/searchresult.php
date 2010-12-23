<link href="<?php echo get_cssdir()?>jquery-ui-1.8.2.custom.css" rel="stylesheet" media="screen" />
<div class="pagepanel">
 <div class="panelhead withpdfbtn">
   <span class="text"><?php echo $report_title?></span>
  <a href="<?php echo $report_link?>"><img src="<?php echo get_imagedir()?>icon_pdf.png" class="floatRight"></a>
   <a href="<?php echo $email_link?>" id="send_mail"><img src="<?php echo get_imagedir()?>emailsend.png" class="floatRight lnsendmail"></a>
   <div class="clearboth"></div>
 </div>
   <div class="panelcontent">
     <div class="tables">
      <?php echo $report_result ;?>
     </div>
    </div>
  </div>
<div title="Send mail" id="dialog" style="display:none">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="to_contacts">To:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="to_contacts" name="to_contacts" />
                     <label for="from_contacts">From:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="from_contacts" name="from_contacts" />
                    <label for="mail_subject">Subject:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="mail_subject" name="mail_subject" />
                    <label for="mail_desc">Message:</label>
                    <textarea class="ui-widget-content ui-corner-all" id="mail_desc" name="mail_desc"></textarea>
                    <input type="hidden" id="parameters"></input>
                </fieldset>
            </form>
  </div>
<script type="text/javascript">
$(document).ready(function() { 
	$('.tables table:first').prepend(
       $('<thead></thead>').append($('.tables tr:first').remove())
       );
    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']}); 
	
	var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 buttons: {
		 'Send': function() {
		 $.ajax({
           type: "POST",
           url: $('#parameters').val(),
           data:({'to':$('#to_contacts').val(),'subject':$('#mail_subject').val(),'message':$('#mail_desc').val(),'from':$('#from_contacts').val()}),
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
	
	$('a#send_mail').click(function(e){	
	  e.preventDefault();
	  $('#parameters').val($(this).attr('href'));
      $dialog.dialog('open');
	});
});
</script>
