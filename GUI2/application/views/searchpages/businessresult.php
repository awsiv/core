<div id="bodyreport" class="outerdiv grid_12">

    <div id="reportpanel" class="innerdiv">
        <p class="title"><?php echo $report_title ?></p>
        <div class="reporthead">
            <a href="<?php echo $report_link ?>" class="icons pdf" title="Generate pdf report"></a>
            <a href="<?php echo $email_link ?>" id="send_mail" class="icons email" title="Send this report as email"></a>
            <!--<a href="<?php echo site_url('search/index/report/' . $report_title) ?>" id="advsearch">Advance search</a>-->
            <?php echo anchor('#', 'Host only', array('id' => 'findhost', 'title' => 'Report for another host')) ?>
        </div>
        <div class="reportpanelcontent">
            <div id="filterdialog" class="hidden"></div>
            <div class="tables tablesfixed">
                <?php
                $result = json_decode($report_result, true);
                if (count($result['data']) > 0) {
                    echo $this->cf_table->generateReportTable($result, $report_title);
                    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
                    include 'paging_footer.php';
                    //echo $report_result;
                } else {
                    echo"<table><tr><td>" . $this->lang->line("no_data") . "</td></tr></table>";
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
            <textarea class="" id="mail_desc" name="mail_desc" rows="4" cols="48"></textarea>
            <input type="hidden" id="parameters" />
        </fieldset>
        <br />
    </form>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        //$('.tables table:first').prepend(
        //$('<thead></thead>').append($('.tables tr:first').remove())
        // );
        $('.tables table').tableFilter();
        $('.tables table').tablesorter({widgets: ['zebra']});


       
        $('#findhost').hostfinder({
            'defaultbehaviour':false,
            'report':'<?php echo $report_title ?>',
            complete:function(event,data){
                //console.log('/search/index/host/'+data.selectedhost+'/report/'+data.report)
                location.replace('<?php echo site_url() ?>/search/index/host/'+data.selectedhost+'/report/'+data.report);
            },
            url: "<?php echo site_url() ?>/widget/hostfinder",
            classhandler:"<?php echo site_url(); ?>/widget/cfclasses"

    });
     
    var $dialog = $('#dialog');
        
    $dialog.dialog({
        autoOpen: false,
        modal: true,
        hide: 'puff',
        width: 'auto',
        buttons: {
            'Send': function() {
                $dialog.append("<div id='tempdiv' class='info'><img src='<?php echo get_imagedir(); ?>ajax-loader.gif' /> sending mail please wait a while...</div>");
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


    $('.reporthead').find('a').qtip({
        style: {
            background:'#454545',
            opacity:0.9,
            name: 'dark',
            width:{
                min:0,
                max:150
            },
            border: {
                width: 1,
                radius: 1
            },
            tip:{
                corner:'bottomMiddle',
                size:{
                    x:5,
                    y:5
                }
            }
        },
        show:{
            effect:{
                type:'fade',
                lenght:200
            }
        },

        position: {
            corner: {
                target: 'topMiddle',
                tooltip: 'bottomMiddle'
            }
        }
    });
});
</script>
