<div class="pagepanel">
 <div class="panelhead withpdfbtn">
   <span class="text"><?php echo $report_title?></span>
   <a href="<?php echo site_url('search/index/report/'.$report_title)?>" id="advsearch">Advance search</a>
   <a href="<?php echo $report_link?>"><img src="<?php echo get_imagedir()?>icon_pdf.png" class="floatRight"></a>
   <a href="<?php echo $email_link?>" id="send_mail"><img src="<?php echo get_imagedir()?>emailsend.png" class="floatRight lnsendmail"></a>
   <div class="clearboth"></div>
 </div>
   <div class="panelcontent">
       <div id="filterdialog" class="hidden"></div>
        <div class="tables">
      <?php
       $result = json_decode($report_result,true);

         $this->table->set_heading(array_keys($result['meta']['header']));
       $heading="";
       if(count($result['data'])>0)
       {
       foreach ($result['data'] as $row)
       {
        //$this->table->add_row($row);
           $temp=array();
           foreach($result['meta']['header'] as $key=>$value)
           {
              if(!is_array($value))
              {
                  if(strtolower($key)=="lastseen"||strtolower($key)=="time" || strtolower($key)=="last verified")
                      array_push($temp,  date('D F d h:m:s Y',$row[$value]));
                  else
                  array_push($temp, $row[$value]);
              }
             else
              {
                    if(strtolower($key)=="note")
                    {
                        $link=site_url("notes").'/' ;
                        $data_index=$value['index'];
                        foreach($value['subkeys'] as $subkey=>$subval)
                        {
                          $data=$row[$data_index][$subval];
                          $link.="$subkey/$data/";
                        }
                      array_push($temp, anchor($link, 'note'));
                         //array_push($temp, 'note');
                    }
              }
           }
          $this->table->add_row($temp);
       }
      echo $this->table->generate();
      $pg = paging($current,$number_of_rows,$result['meta']['count'],100);

      //$this->table->set_heading($keys);
      //foreach ($result as $key=>$value)
      //{

      //}
//echo $report_result .'<br />';
      //echo json_last_error();
     // print_r($result);
      //print_r($heading);
      ?>
       <div class="Paging">
           <div>
               <?
               echo form_open('search/index/'.$params);
                echo   form_input('rows', $number_of_rows);
                echo "Rows/Page";
                 echo form_close();
                ?>
           </div>
           <div class="pages">
                                   <div class="inside">
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['first'])?>" title="Go to First Page" class="first"><span>&laquo;</span></a>
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['prev'])?>" title="Go to Previous Page" class="prev"><span>&lsaquo;</span></a>

                                    <? for ($i=$pg['start'];$i<=$pg['end'];$i++) {
                                       if ($i==$pg['page']) $current = 'current'; else $current="";
                                    ?>

                                    <a href="<?=site_url("search/index/".$params."rows/$number_of_rows/page/$i")?>" title="Go to Page <?=$i?>" class="page <?=$current?>"><span><?=$i?></span></a>

                                    <? } ?>

                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['next'])?>" title="Go to Next Page" class="next"><span>&rsaquo;</span></a>
                                    <a href="<?=site_url('search/index/'.$params.'page/'.$pg['last'])?>" title="Go to Last Page" class="last"><span>&raquo;</span></a>
                                    </div>
           </div>
    </div>
       <?php
       }
       else{
         echo"<table><tr><td>NO Data Found !!</td></tr></table>";
       }
       ?>
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
                    <input type="hidden" id="parameters" />
                </fieldset>
            </form>
  </div>
<script type="text/javascript">
$(document).ready(function() {
	//$('.tables table:first').prepend(
       //$('<thead></thead>').append($('.tables tr:first').remove())
       //);
   $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']});

        $('#advsearch').click(function(event){
            event.preventDefault();
             $('#filterdialog').load('/search/index', {'report':'<?php echo $report_title ?>'}, function() {
              var $dialog=$(this);
               var $closebtn=$("<a class='ui-panel-close'><span class='ui-icon ui-icon-closethick'></span></a>");
               $(this).slideDown('slow');
                $(this).find('.panelhead').append($closebtn);
                 $closebtn.hover(function () {
                             $(this).addClass("ui-state-hover");
                        },
                           function () {
                             $(this).removeClass("ui-state-hover");
                         });
                  $closebtn.click(function(){
                   $dialog.fadeOut();
                 });
               });
        });

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
