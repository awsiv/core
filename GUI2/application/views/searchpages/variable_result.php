
<div id="bodyreport" class="outerdiv">
    <div id="reportpanel" class="innerdiv">
        <p class="title"><?php echo $report_title ?></p>
        <div class="reporthead">
            <div class="grid_8">
                <a href="<?php echo $report_link ?>" class="icons pdf"></a>
                <a href="<?php echo $email_link ?>" id="send_mail" class="icons email"></a>
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
        </div>

        <div class="clearboth">
            <div id="modifySearchPanel" style="">

            </div>
            <div class="tables tablesfixed">
                <?php
                $result = json_decode($report_result, true);
                //$this->table->set_heading(array_keys($result['meta']['header']))
                if (key_exists('truncated', $result['meta'])) {
                    $message = $result['meta']['truncated'];
                    echo "<p class=\"info\">$message</p>";
                }
                if ($result['meta']['count'] > 0) {
                    echo "<h2> Total : " . $result['meta']['count'] . " variables found </h2>";
                    foreach ($result as $bundles => $variables) {
                        if ($bundles != "meta") {
                            echo "<h2> bundle $bundles: <i>" . $variables['count'] . " variables</i></h2>";
                            $this->table->set_heading(array_keys($variables['header']));
                            foreach ($variables['data'] as $row) {
                                //$this->table->add_row($row);
                                $temp = array();
                                foreach ($variables['header'] as $key => $value) {
                                    if (!is_array($value)) {
                                        if (strtolower($key) == "lastseen" || strtolower($key) == "time")
                                            array_push($temp, date('D F d h:m:s Y', $row[$value]));
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
                    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
                    //echo $report_result .'<br />';
                    //echo json_last_error();
                    // print_r($result);
                    //print_r($heading);
                    include 'paging_footer.php';
                }
                else {
                    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
                }
                ?>

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

