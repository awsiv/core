
<div id="bodyreport" class="outerdiv grid_12">
    <div id="reportpanel" class="innerdiv">
        <p class="title"><?php echo $report_title ?></p>
        <div class="reporthead">
            <a href="<?php echo $report_link ?>" class="icons pdf"></a>
            <a href="<?php echo $email_link ?>" id="send_mail" class="icons email"></a>
            <?php echo anchor('#', 'Host only', array('id' => 'findhost')) ?>
        </div>
        <div class="tables">
            <?php
            $result = json_decode($report_result, true);
            $headings = array();
            foreach ($result['meta']['header'] as $key => $value) {
                if (strtolower($key) == "change") {
                    $column = $key . " Added(+) , Deleted(-) , line No, Content";
                    array_push($headings, $column);
                } else {
                    array_push($headings, $key);
                }
            }
            //$this->table->set_heading(array_keys($result['meta']['header']));
            $this->table->set_heading($headings);
            $heading = "";
            if (count($result['data']) > 0) {
                foreach ($result['data'] as $row) {
                    //$this->table->add_row($row);
                    $temp = array();
                    foreach ($result['meta']['header'] as $key => $value) {
                        if (!is_array($value)) {
                            if (strtolower($key) == "change detected at")
                                array_push($temp, date('D F d h:m:s Y', $row[$value]));
                            else
                                array_push($temp, $row[$value]);
                        }
                        else {
                            if (strtolower($key) == "change") {
                                $table = "<table>"; /* generate subkeys for inner table */;
                                $subindexs = array_values($value['subkeys']);
                                $index = $value['index'];

                                foreach ($row[$index] as $subrows) {
                                    $table.="<tr>";
                                    $minus = false;
                                    foreach ($subindexs as $subindex) {
                                        $cell = $subrows[$subindex];
                                        if ($cell == "-" && $subindex == 0) {
                                            $minus = true;
                                        }

                                        if ($minus && $subindex == 2) {
                                            $table.="<td class=\"minus\">$cell</td>";
                                        } else {
                                            $table.="<td>$cell</td>";
                                        }
                                    }
                                    $table.="</tr>";
                                }
                                $table.="</table>";
                                array_push($temp, $table);
                            }
                        }
                    }
                    $this->table->add_row($temp);
                }
                echo $this->table->generate();
                $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);

                //$this->table->set_heading($keys);
                //foreach ($result as $key=>$value)
                //{
                //}
                //echo $report_result . '<br />';
                //echo json_last_error();
                // print_r($result);
                //print_r($heading);
               include 'paging_footer.php';
                    }else {
                        echo"<table><tr><td>" . $this->lang->line("no_data") . "</td></tr></table>";
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
            <input type="text" class="ui-widget-content ui-corner-all" value="" id="from_contacts" name="from_contacts"  size="50" />
            <label for="mail_subject">Subject:</label>
            <input type="text" class="ui-widget-content ui-corner-all" value="" id="mail_subject" name="mail_subject"  size="50" />
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
        $('.tables table').tableFilter();
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

        $('#findhost').hostfinder({
            'defaultbehaviour':false,
            'report':'<?php echo $report_title ?>',
            complete:function(event,data){
                //console.log('/search/index/host/'+data.selectedhost+'/report/'+data.report)
                location.replace('/search/index/host/'+data.selectedhost+'/report/'+data.report);
            }

        });
    });
</script>

