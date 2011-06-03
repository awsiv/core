
<div id="bodyreport" class="outerdiv">
    <div id="reportpanel" class="innerdiv">
                      <p class="title"><?php echo $report_title ?></p>
     <div class="reporthead">
       <a href="<?php echo $report_link ?>" class="icons pdf"></a>
       <a href="<?php echo $email_link ?>" id="send_mail" class="icons email"></a>
     </div>
     <div class="tables tablesfixed">
      <?php
       $result = json_decode($report_result,true);
       //$this->table->set_heading(array_keys($result['meta']['header']))
       foreach($result as $bundles=>$variables)
       {
           if($bundles !="meta")
           {
             echo "<h2> bundle $bundles: <i>total".$variables['count']." variables</i></h2>";
             $this->table->set_heading(array_keys($variables['header']));
             foreach ($variables['data'] as $row)
              {
              //$this->table->add_row($row);
               $temp=array();
               foreach($variables['header'] as $key=>$value)
               {
                if(!is_array($value))
                 {
                  if(strtolower($key)=="lastseen"||strtolower($key)=="time")
                      array_push($temp,  date('D F d h:m:s Y',$row[$value]));
                  else
                  array_push($temp, $row[$value]);
                 }
                }
                $this->table->add_row($temp);
               }
             echo $this->table->generate();
             }
             $this->table->clear();
       }
      $pg = paging($current,$number_of_rows,$result['meta']['count'],10);
     // echo $report_result .'<br />';
      //echo json_last_error();
     // print_r($result);
      //print_r($heading);
      ?>
     </div>
       <div class="Paging">
           <div class="pages">
                                   <div class="inside">
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['first'])?>" title="Go to First Page" class="first"><span>First</span></a>
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['prev'])?>" title="Go to Previous Page" class="prev"><span><</span></a>

                                    <? for ($i=$pg['start'];$i<=$pg['end'];$i++) {
                                       if ($i==$pg['page']) $current = 'current'; else $current="";
                                    ?>

                                    <a href="<?=site_url("search/index/".$params."rows/$number_of_rows/page/$i")?>" title="Go to Page <?=$i?>" class="page <?=$current?>"><span><?=$i?></span></a>

                                    <? } ?>

                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['next'])?>" title="Go to Next Page" class="next"><span>></span></a>
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['last'])?>" title="Go to Last Page" class="last"><span>Last</span></a>
                                    </div>
           </div>
           <div>
               <?
               echo form_open('search/index/'.$params);
                echo   form_input('rows', $number_of_rows);
                echo "Rows/Page";
                 echo form_close();
                ?>
           </div>
    </div>
</div>
</div>
<div title="Send mail" id="dialog"  class="stylized" style="display:none">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="to_contacts">To:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="to_contacts" name="to_contacts" size="50" />
                     <label for="from_contacts">From:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="from_contacts" name="from_contacts" size="50" />
                    <label for="mail_subject">Subject:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="mail_subject" name="mail_subject" size="50" />
                    <label for="mail_desc">Message:</label>
                    <textarea class="ui-widget-content ui-corner-all" id="mail_desc" name="mail_desc" rows="4" cols="48"></textarea>
                    <input type="hidden" id="parameters" />
                </fieldset>
                <br />
            </form>
  </div>
<script type="text/javascript">
$(document).ready(function() {
	//$('.tables table:first').prepend(
       //$('<thead></thead>').append($('.tables tr:first').remove())
       //);
  //$('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']});

	var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
                 width:'auto',
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

