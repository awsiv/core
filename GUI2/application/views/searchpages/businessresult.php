<div id="bodyreport" class="outerdiv grid_12">

    <div id="reportpanel" class="innerdiv">
        <?php $this->load->view('/searchpages/reportsMenu'); ?>
        <div class="title expanded" style="text-align:center" id="show_fileter_form">
            <div><em>&nbsp;</em><span><?php echo $report_title; ?></span><em>&nbsp;</em></div>
            <div class="clear"></div>
        </div>
        <script type="text/javascript">
            $('#show_fileter_form').click(function() {
                $('#modifySearchPanel').toggle(0,function(){
                    $('#reportpanel .title').toggleClass('collapsed');
                });
            });
        </script>    
        <div id="modifySearchPanel">
            <div class="grid_7">
                <?php $this->load->view('searchpages/' . $filter_view); ?> 

            </div>
            <div id="savesearchcontainer" class="grid_5 floatright panelcontent"> 
                <div id="searchSaveError" class="error" style="display:none;"></div>
                <div id="searchSaveSuccess" class="success" style="display:none;"></div> 
                <form id="saveform" method="post" action="<?php echo site_url(); ?>/savedsearch/save/">
                    <p>
                        <label>Save this search </label> 
                        <input type="input" id="search_name" name="search_name" class="textbox"></input>
                        <span class="green_btn"><input type="submit" id="submit_search" value="save"/></span>
                        <span class="clear"></span>
                    </p>
                    <input type="hidden" id="search_params" name="search_params" value="<?php echo $params; ?>"></input>
                    <input type="hidden" id="search_url" name="search_url" value="<?php echo site_url() . $breadCrumbUrl; ?>"></input>
                    <input type="hidden" id="report_title" name="report_title" value="<?php echo $report_type; ?>"></input>

                </form>
          
                <div class="alignrightbuttoncontainer">
                    <a href="<?php echo site_url("/savedsearch/listSavedSearches/$report_type"); ?>"  class="green_btn loadsavedsearch"><span>Load saved searches</span></a>
                </div>
            </div> 
            <div class="clear"></div> 
        </div>
 
        <div class="reportpanelcontent">
            <div class="grid_7">
                <span id="count_hosts">Report for XXXX hosts</span>
                <a href="<?php echo site_url('widget/contextfinder') ?>" id="hclist" class="green_btn" title="<?php echo $this->lang->line('report_hostgp_help'); ?>"><span>Hostcontext</span></a>
                <script type="text/javascript">
                    var $incList = $('#searchform input:hidden[name=inclist]');
                    var $exList  = $('#searchform input:hidden[name=exlist]');
                    $('#hclist').contextfinder({
                        baseUrl: '<?php echo site_url() ?>',
                        complete:function(event,data){
                            $incList.val(data.includes);
                            $exList.val(data.excludes);
                        }
                    }); 

                    $('#hclist').contextfinder('setContext',$incList.val().split(','), $exList.val().split(','));

                    var obj = { name: 'filterview' };
                    mediator.installTo(obj); 
                    obj.publish('contextChange', { includes:stringToArray($incList.val()),excludes:stringToArray($exList.val())});

                    obj.subscribe('contextChange', function(data){
                        $incList.val(data.includes);
                        $exList.val(data.excludes);
                    });

                </script> 
            </div>    
            <div id="totalResults" class=" grid_5" style="text-align: right;">
                <span id="total_result">Total results found: <?php echo $report_result['meta']['count']; ?></span>
                <?php if(is_array($paramArray) && !isset($paramArray['hosts_only'])){?>
                    <a href="<?php echo $report_link ?>" id="send_mail" class="green_btn showqtip" title="<?php echo $this->lang->line('tool_tip_download_report') ?>"><span><em class="download_ico">&nbsp;</em>Download</span></a>
               <?php }?>
                
            </div>
            <div class="clear"></div> 
            <div id="mainResultTable">
                <?php 
                    include_once ($resultView . '.php');
                ?> 
            </div>    
        </div>
    </div>
</div>
<div class="clear"></div>
<div title="Report download" id="dialog" class="stylized" style="width:400px;display:none;">
    <form>
        <fieldset class="ui-helper-reset">
            <label for="report_download">Download: </label>
            <select id="report_download" name="report_download">
                <option value="1" selected>All</option>
                <option value="0">Current page only</option>
            </select>    
            <br/>
            <label for="report_type">Report type: </label>
            <select id="type" name="type">
                <option value="pdf" selected>pdf</option>
                <option value="csv">csv</option>
            </select> 
            <br/>
            <label for="sendemail" >Send in email: </label>
            <input type="checkbox" id="sendemail" value="email" name="sendemail" onclick="$('#emailfieldset').toggle();">
        </fieldset>
        <br/>
        <fieldset id="emailfieldset" style="display: none; margin-top: 10px">
            <legend><span style="padding: 3px; background: #4A575C; font-size: 11px; border-radius: 5px 5px 5px 5px;color: #fff">
                    Email details<span>
                        </legend>
                        <label for="to_contacts">To: </label>
                        <input type="text"  value="" id="to_contacts" name="to_contacts"  size="50"/>
                        <label for="from_contacts">From: </label>
                        <input type="text" class="" value="<?php echo $fromEmail ?>" id="from_contacts" name="from_contacts" size="50" />
                        <label for="mail_subject">Subject: </label>
                        <input type="text" class="" value="" id="mail_subject" name="mail_subject"  size="50" />


                        <label for="mail_desc">Message: </label>
                        <textarea class="" id="mail_desc" name="mail_desc" rows="4" cols="43"></textarea>

                        <input type="hidden" id="parameters" value="<?php echo $report_link ?>" />
                        </fieldset>
                        <br />
                        </form>

                        <div id="loading" class="info" style="display: none;"><img src='<?php echo get_imagedir(); ?>ajax-loader.gif' />Please wait a while...</div>
                        <div id="tempdiv" style="display: none;" ></div>

                        </div>
<script type="text/javascript">
    $(document).ready(function() {


        //$("#searchform").addClass('grid_8');

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
            'baseUrl':'<?php echo site_url() ?>',
            'report':'<?php echo $hostfinderparams ?>',
            complete:function(event,data){
                location.replace('<?php echo site_url() ?>/search/index/host/'+data.selectedhost+'/'+data.report);
            }
        });

        var $dialog = $('#dialog');

        $dialog.dialog({
            autoOpen: false,
            modal: true,
            width: 'auto',
            buttons: {
                'Ok': function() {
                    $('#tempdiv').html(''); 
                    $('#loading').show();                                            

                    $.ajax({
                        type: "POST",
                        url: $('#parameters',$dialog).val(),
                        data:
                            {
                            'rf':$('#type',$dialog).val(),
                            'to':$('#to_contacts',$dialog).val(),
                            'subject':$('#mail_subject',$dialog).val(),
                            'message':$('#mail_desc',$dialog).val(),
                            'from':$('#from_contacts',$dialog).val(),
                            'pdfaction': $('#sendemail',$dialog).is(':checked') ? $('#sendemail',$dialog).val() : null, 
                            'page':'<?php echo $current; ?>',
                            'rows':'<?php echo $number_of_rows; ?>',
                            'download':$('#report_download',$dialog).val()
                        },
                        dataType:'json',
                        async: true,
                        success: function(data){
                            $('#loading',$dialog).slideUp(); 
                            $('#tempdiv',$dialog).show().removeClass('error');
                            $('#tempdiv',$dialog).addClass('info');
                            $('#tempdiv',$dialog).append(data.message); 
                            $(":button:contains('Send')").hide();
                            $('#tempdiv').find('a').live('click',function() {
                                $dialog.dialog('close');
                            }); 

                        },
                        error: function(jqXHR, textStatus, errorThrown){
                            $('#loading',$dialog).slideUp(); 
                            $('#tempdiv',$dialog).removeClass('info');
                            $('#tempdiv',$dialog).addClass('error');                                
                            $('#tempdiv',$dialog).show().html(errorThrown);
                        }
                    });

                },
                'Cancel': function() {
                    $('#loading',$dialog).hide(); 
                    $('#tempdiv',$dialog).html('');
                    $('#tempdiv',$dialog).addClass('info');
                    $(this).dialog('close');
                }
            },
            open: function() {
                $('form',$dialog).show();
                $('#to_contacts',$dialog).focus();
                $('#tempdiv',$dialog).html('').hide();
                $(":button:contains('Send')").show();
                $(":button:contains('Cancel')").show();
                $(":button:contains('Ok')").show();
            },
            close: function() {

            }
        });

        $('a#send_mail').click(function(e){
            e.preventDefault();
            $('#parameters',$dialog).val($(this).attr('href'));
            $dialog.dialog('open');
        });



        $("#mainResultTable").on("click","a.note", function(event) {
                                event.preventDefault();
                                $(this).ajaxyDialog({
                                title:'Notes',
                                dontOverrideTitle:true,
                                change:function(nid,element) {
                                            // change the url
                                        $(element).attr('href', '/notes/index/action/show/nid/' + nid);
                                        }
                                 });
                                 $(this).ajaxyDialog("open");
          });

    });
</script>
