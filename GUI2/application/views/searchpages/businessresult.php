<?php //echo $breadCrumbUrl;              ?>
<div id="bodyreport" class="outerdiv grid_12">

    <div id="reportpanel" class="innerdiv">
        <p class="title"><?php echo $report_title; echo (isset($hostname)&&($hostname!=""))?" for ".$hostname:"";?></p>
        <div class="reporthead">
            <div class="grid_8">
                <a href="<?php echo $report_link ?>" class="icons pdf showqtip" title="Generate pdf report"></a>
                <a href="<?php echo $email_link ?>" id="send_mail" class="icons email showqtip" title="Send this report as email"></a>
                <!--<a href="<?php echo site_url('search/index/report/' . $report_title) ?>" id="advsearch">Advance search</a>-->
                <?php echo anchor('#', 'Select Host', array('id' => 'findhost', 'title' => 'Report for another host', 'class' => 'showqtip')) ?>
            </div>
            <div class="grid_4" style="text-align: right;">
                <div><a href="#" id="savesearch" class="showqtip" title="save this search for future use"><span class="ui-icon-triangle-1-s"></span>Save this search</a></div>
                <div id="savesearchcontainer" style="margin: 5px;display:none;">
                    <div id="searchSaveError" class="error" style="display:none;"></div>
                    <div id="searchSaveSuccess" class="success" style="display:none;"></div> 
                    <form id="saveform" method="post" action="<?php echo site_url(); ?>/savedsearch/save/">
                        <label for="search_name">Name</label>   
                        <input type="input" id="search_name" name="search_name" class="textbox"></input>
                        <input type="hidden" id="search_url" name="search_url" value="<?php echo $params; ?>"></input>
                        <input type="hidden" id="report_title" name="report_title" value="<?php echo $report_title; ?>"></input>
                        <input type="submit" id="submit_search" value="save" class="btn" />
                    </form>                
                </div> 
                <div><a href="#" id="modifySearch" class="showqtip" title="modify search parameters">New search</a></div>

            </div>
            <div class="clearboth">
            </div>   

        </div>
        <div id="modifySearchPanel" style="">

        </div>

        <div class="reportpanelcontent">
            <div id="filterdialog" class="hidden"></div>
            <div class="tables tablesfixed">
                <?php
                $result = json_decode($report_result, true);
                if (key_exists('truncated', $result['meta'])) {
                    $message = $result['meta']['truncated'];
                    $displayed_rows = count($result['data']);
                    echo "<p class=\"info\">$message. Queried for :<strong>$number_of_rows rows</strong> , displayed:<strong> $displayed_rows rows</strong>. Please go to next page for more results</p>";
                    $number_of_rows = $displayed_rows;
                }
                if (count($result['data']) > 0) {
                    echo "Total results found: " . $result['meta']['count'];
                    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
                    echo $this->cf_table->generateReportTable($result, $report_title);
                    include 'paging_footer.php';
                } else {
                    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
                }
                ?> 
            </div>
        </div>
    </div>
</div>
<div class="clear"></div>
<div title="Send mail" id="dialog" class="stylized" style="width:400px;display:none">
    <form>
        <fieldset class="ui-helper-reset">
            <label for="to_contacts">To:</label>
            <input type="text"  value="" id="to_contacts" name="to_contacts"  size="50"/>
            <label for="from_contacts">From:</label>
            <input type="text" class="" value="" id="from_contacts" name="from_contacts" size="50" />
            <label for="mail_subject">Subject:</label>
            <input type="text" class="" value="" id="mail_subject" name="mail_subject"  size="50" />
            <label for="mail_desc">Message:</label>
            <textarea class="" id="mail_desc" name="mail_desc" rows="4" cols="43"></textarea>
            <input type="hidden" id="parameters" />
        </fieldset>
        <br />
    </form>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        // $('.tables table').tableFilter();
        $('.tables table').tablesorter({widgets: ['zebra']});
        
        
        $('#modifySearch').click(function(e) {
            e.preventDefault();  
            $('#modifySearchPanel').toggle();
            if(!$("#modifySearchPanel").is(":hidden")) {
                var url = '<?php echo site_url(); ?>/search/index';
                var postData = {'report':'<?php echo $report_title; ?>'};
                $.post(url,postData, function(data) {
                    $('#modifySearchPanel').html(data);                   
                });
            }
        });

        // for save search 
        $('#savesearch').click(function(e) {
            e.preventDefault();            
            $('#savesearchcontainer').toggle('slow',function() {
                
                if (!$('#savesearchcontainer').is(':visible')) {
                    $('#searchSaveSuccess').html(' ').hide();
                    $('#searchSaveError').html(' ').hide();
                }
            });
           
            return false;
        });
        
        
        $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});
        
        
        
        $('#saveform').ajaxForm(
        {
            target:        '#searchSaveSuccess',
            success:function(data) {
                $('#searchSaveSuccess').show();
                $('#searchSaveError').hide();

            },
            error:function(xhr, ajaxOptions, thrownError) {
                $('#searchSaveSuccess').hide()
                $('#searchSaveError').html(xhr.responseText);
                $('#searchSaveError').show();
            }
        }
    );
        
        
        $('#findhost').hostfinder({
            'defaultbehaviour':false,
            'report':'<?php echo $hostfinderparams ?>',
            complete:function(event,data){
                //console.log('/search/index/host/'+data.selectedhost+'/report/'+data.report)
                location.replace('/search/index/host/'+data.selectedhost+'/'+data.report);
            }

        });
        
       
        var $dialog = $('#dialog');
        
        $dialog.dialog({
            autoOpen: false,
            modal: true,
            width: 'auto',
            buttons: {
                'Send': function() {
                    $('#tempdiv',$dialog).remove(); 
                    $dialog.append("<div id='tempdiv' class='info'><img src='<?php echo get_imagedir(); ?>ajax-loader.gif' /> sending mail please wait a while...</div>");
                    $.ajax({
                        type: "POST",
                        url: $('#parameters',$dialog).val(),
                        data:({'to':$('#to_contacts',$dialog).val(),'subject':$('#mail_subject',$dialog).val(),'message':$('#mail_desc',$dialog).val(),'from':$('#from_contacts',$dialog).val()}),
                        dataType:'json',
                        async: false,
                        success: function(data){
                            $('form',$dialog).hide();
                            $('#tempdiv',$dialog).removeClass('error');
                            $('#tempdiv',$dialog).addClass('info');
                            $('#tempdiv',$dialog).html(''); 
                            $('#tempdiv',$dialog).html(data.message); 
                            $(":button:contains('Send')").hide();
                            $(":button:contains('Cancel')").hide();
                        },
                        error: function(jqXHR, textStatus, errorThrown){
                            $('#tempdiv',$dialog).removeClass('info');
                            $('#tempdiv',$dialog).addClass('error');                                
                            $('#tempdiv',$dialog).html(errorThrown);
                        }
                    });
                   
                },
                'Cancel': function() {
                    $(this).dialog('close');
                }
            },
            open: function() {
                $('form',$dialog).show();
                $('#to_contacts',$dialog).focus();
                $('#tempdiv',$dialog).remove();
                $(":button:contains('Send')").show();
                $(":button:contains('Cancel')").show();
                 

            },
            close: function() {

            }
        });

        $('a#send_mail').click(function(e){
            e.preventDefault();
            $('#parameters',$dialog).val($(this).attr('href'));
            $dialog.dialog('open');
        });



        $('.note').ajaxyDialog({
            title:'Notes',
            dontOverrideTitle:true,
            change:function(nid,element) {

                // change the url
                $(element).attr('href', '/notes/index/action/show/nid/' + nid);
                // console.log(nid,element);
            }});
       
    });
</script>
