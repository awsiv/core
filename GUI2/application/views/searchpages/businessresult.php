<div id="bodyreport" class="outerdiv grid_12">

    <div id="reportpanel" class="innerdiv">
        <?php $this->load->view('/searchpages/reportsMenu'); ?>
        <table class="grid_12 reports_header" style="margin: 10px">
            <tr>
                <td width="150" class="no_wrap">
                    <span id="count_hosts">Report for <?php echo $report_result['meta']['related']; if ($report_result['meta']['related'] == 1) echo " host "; else echo " hosts "; ?>(out of <?php echo $hostcount ?> from selected host context)</span> 
                    <a href="<?php echo site_url('widget/contextfinder') ?>" id="hclist" class="showqtip host_context_btn" title="<?php echo $this->lang->line('report_hostgp_help'); ?>"></a>
                </td>
                <td id="show_filter_form" class="showqtip" title="Click to toggle more filters" style="width: 89%">
                    <div class="title">
                        <em class="front">&nbsp;</em><span style="font-weight:bold"><?php echo $report_title; ?></span><em  class='back'>&nbsp;</em>
                    </div>  
                </td>
            </tr>                
        </table>     
       
        <div class="clear"></div>
        <div id="modifySearchPanel">
            <div class="grid_7_omega reportForm">
                <?php 
                if ($filter_view != '') {
                    $this->load->view('searchpages/' . $filter_view);
                }
                ?> 
            </div>
            <script type="text/javascript">
            $('#show_filter_form').click(function() {
                $('#modifySearchPanel').toggle(0,function(){
                    $('#reportpanel .title').toggleClass('collapsed');
                });
            });
           </script>  
            <div id="savesearchcontainer" class="grid_5_alpha"> 
                <div style="border-left:solid 1px #fff; padding:0 0 0 15px">
                <div id="searchSaveError" class="error" style="display:none;"></div>
                <div id="searchSaveSuccess" class="success" style="display:none;"></div> 
                <form id="saveform" method="post" action="<?php echo site_url(); ?>/savedsearch/save/">
                    <p>
                        <label>Save this search </label> 
                        <input type="text" id="search_name" name="search_name" class="textbox" size="31"></input>
                        <span class="green_btn"><input type="submit" id="submit_search" value="Save"/></span>
                        <span class="clear"></span>
                    </p>
                    <input type="hidden" id="search_params" name="search_params" value="<?php echo $params; ?>"></input>
                    <input type="hidden" id="search_url" name="search_url" value="<?php echo $breadCrumbUrl; ?>"></input>
                    <input type="hidden" id="report_title" name="report_title" value="<?php echo $report_title; ?>"></input>

                </form>
               </div>
            </div> 
            <div class="clear"></div> 
        </div>
 
        <div class="reportpanelcontent">
            <div class="grid_3">
              <span id="total_result">Total results: <?php echo $report_result['meta']['count']; ?></span>
            </div>    
            <div id="totalResults" style="text-align: right;">
               <?php if ($report_result['meta']['count'] > 0 ) {
                  $pg = paging($current, $number_of_rows, $report_result['meta']['count'], 5);
                  include 'paging_footer.php';
                  ?>
                    <?php if (isset($report_link) && !empty($report_link)) { ?>
                        <a href="<?php echo $report_link ?>" id="send_mail" class="showqtip" title="<?php echo $this->lang->line('tool_tip_download_report') ?>"><span><em class="download_ico">&nbsp;</em></span></a>
                        <?php } ?>
               <?php } ?>
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
<?php if (isset($report_link) && !empty($report_link)) { ?>
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
            <legend><span>Email details</span></legend>
                        <label for="to_contacts">To: </label>
                        <input type="text"  value="" id="to_contacts" name="to_contacts"  size="50"/>
                        <label for="from_contacts">From: </label>
                        <input type="text" class="" value="<?php echo $fromEmail ?>" id="from_contacts" name="from_contacts" size="50" />
                        <label for="mail_subject">Subject: </label>
                        <input type="text" class="" value="<?php echo $report_title; ?>" id="mail_subject" name="mail_subject"  size="50" />


                        <label for="mail_desc">Message: </label>
                        <textarea class="" id="mail_desc" name="mail_desc" rows="4" cols="43"></textarea>

                        <input type="hidden" id="parameters" value="<?php echo $report_link ?>" />
                        </fieldset>
                        <br />
                        </form>

                        <div id="loading" class="info" style="display: none;"><img src='<?php echo get_imagedir(); ?>ajax-loader.gif' />Please wait a while...</div>
                        <div id="tempdiv" style="display: none;" ></div>

                        </div>
<?php } ?>
<script type="text/javascript">
    $(document).ready(function() {
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





        $('#saveform').ajaxForm(
        {
            target:        '#searchSaveSuccess',
            success:function(data) {
                $('#searchSaveSuccess').show().delay(10000).fadeOut(400);
                $('#searchSaveError').hide().delay(10000).fadeOut(400);
                $('#search_name').val('');

            },
            error:function(xhr, ajaxOptions, thrownError) {
                $('#searchSaveSuccess').hide().delay(10000).fadeOut(400);
                $('#searchSaveError').html(xhr.responseText);
                $('#searchSaveError').show().delay(10000).fadeOut(400);
            }
        }
    );



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
                            if (typeof(data) !== 'undefined' && data != null) {
                                $('#loading',$dialog).slideUp();
                                $('#tempdiv',$dialog).show().removeClass('error');
                                $('#tempdiv',$dialog).addClass('info');
                                $('#tempdiv',$dialog).append(data.message);
                                $(":button:contains('Send')").hide();
                                $('#tempdiv').find('a').live('click',function() {
                                    $dialog.dialog('close');
                                });
                            } else {
                                     // we are not getting anything from server, must probably an error case
                                    var  error = "Something went wrong while generating this report.Please check the logs for more details.";
                                    $('#loading',$dialog).slideUp();
                                    $('#tempdiv',$dialog).removeClass('info');
                                    $('#tempdiv',$dialog).addClass('error');
                                    $('#tempdiv',$dialog).show().html(error);
                            }

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



          
            var $incList = $('#searchform input:hidden[name=inclist]');
            var $exList  = $('#searchform input:hidden[name=exlist]');
                    
                    $('#hclist').contextfinder({
                        title: '<?php echo $this->lang->line('report_hostgp_help'); ?>',
                        baseUrl: '<?php echo site_url() ?>',
                        include_field_name: 'include',
                        exclude_field_name: 'exclude',
                        
                        setContextClbkFnc:function() { 
                            $('.reportForm form').submit();
                        }, 
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

    });
</script>
