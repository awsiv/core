<div id="bodyreport">
    
              <div id="reportpanel" class="innerdiv">
                  <p class="title"><?php echo $report_title ?></p>
    <div class="reporthead">
        <a href="<?php echo $report_link ?>" class="icons pdf"></a>
        <a href="<?php echo $email_link ?>" id="send_mail" class="icons email"></a>
        <!--<a href="<?php echo site_url('search/index/report/' . $report_title) ?>" id="advsearch">Advance search</a>-->
        <?php echo anchor('#','Host only',array('id'=>'findhost')) ?>
    </div>
    <div class="reportpanelcontent">
        <div id="filterdialog" class="hidden"></div>
        <div class="tables">
            <?php
            $result = json_decode($report_result, true);
            if (count($result['data']) > 0) {
                echo $this->cf_table->generateReportTable($result, $report_title);
                $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
            ?>
                <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?= site_url('search/index/' . $params . 'page/' . $pg['first']) ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?= site_url('search/index/' . $params . 'page/' . $pg['prev']) ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $current = 'current'; else
                                $current="";
                        ?>

                            <a href="<?= site_url("search/index/" . $params . "rows/$number_of_rows/page/$i") ?>" title="Go to Page <?= $i ?>" class="page <?= $current ?>"><span><?= $i ?></span></a>

                        <? } ?>

                        <a href="<?= site_url('search/index/' . $params . 'page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?= site_url('search/index/' . $params . 'page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>
                <div>
                    <?php
                    echo form_open('search/index/' . $params);
                    echo form_input('rows', $number_of_rows);
                    echo "Rows/Page";
                    echo form_close();
                    ?>
                </div>
            </div>
            <?php
                    }
                    else {
                        echo"<table><tr><td>" . $this->lang->line("no_data") . "</td></tr></table>";
                    }
            ?> 
            </div>
        </div>
 </div>
    </div>
<div class="clear"></div>
        <div title="Send mail" id="dialog" style="width:400px;display:none">
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
               // );
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

     $('#findhost').hostfinder({
           'defaultbehaviour':false,
           'report':'<?php echo $report_title?>',
             complete:function(event,data){
                 //console.log('/search/index/host/'+data.selectedhost+'/report/'+data.report)
                  location.replace('/search/index/host/'+data.selectedhost+'/report/'+data.report);
             }

     });
     
        var $dialog = $('#dialog');
        
        $dialog.dialog({
            autoOpen: false,
            modal: true,
            hide: 'puff',
            width: 'auto',
            buttons: {
                'Send': function() {
                    $dialog.append("<div id='tempdiv' class='amber'>sending mail please wait a while...</div>");
                    $.ajax({
                        type: "POST",
                        url: $('#parameters',$dialog).val(),
                        data:({'to':$('#to_contacts',$dialog).val(),'subject':$('#mail_subject',$dialog).val(),'message':$('#mail_desc',$dialog).val(),'from':$('#from_contacts',$dialog).val()}),
                        dataType:'json',
                        async: false,
                        success: function(data){
                            alert(data.message);
                            $dialog.dialog('close');
                            $('#tempdiv',$dialog).remove();

                        },
                        error: function(jqXHR, textStatus, errorThrown){
                            alert(textStatus);
                            $('#tempdiv',$dialog).remove();
                        }
                    });
                   
                },
                'Cancel': function() {
                    $(this).dialog('close');
                }
            },
            open: function() {
                $('#to_contacts',$dialog).focus();

            },
            close: function() {

            }
        });

        $('a#send_mail').click(function(e){
            e.preventDefault();
            $('#parameters',$dialog).val($(this).attr('href'));
            $dialog.dialog('open');
        });



        $('.note').ajaxyDialog({change:function(nid,element) {

                // change the url
               $(element).attr('href', '/notes/index/action/show/nid/' + nid);
               // console.log(nid,element);
        }});



    });
</script>
