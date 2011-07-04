<div id="body">
    <div class="outerdiv">
        <div class="innerdiv" id="collapse">
            <h3><a href="#">Search</a></h3>
            <div id="notes-filter" style="padding:5px;">
                <div class="stylized">
                    <form action="/notes/shownotes" method="POST">
                        <fieldset>                           
                            <label id="Username" for="username">User Name :: </label>
                            <input type="text" name="username" value="<?php echo set_value('username'); ?>" size="50" />

                            <label for="date_from">Date From :: </label>
                            <input autocomplete="off" id="date_from" type="text" name="date_from" value="<?php echo set_value('date_from'); ?>" size="50" />

                            <label for="date_to">Date To :: </label>
                            <input autocomplete="off" id="date_to" type="text" name="date_to" value="<?php echo set_value('date_to'); ?>" size="50" />
                            <label for="submit"></label>
                            <input type="submit" value="search" name="submit" />
                        </fieldset>

                        <br />
                    </form>
                </div>
            </div>
        </div>
        <?php 
        if ($filters['userId'] != '' || $filters['dateFrom'] != '-1' || $filters['dateTo'] != '-1') { ?>
            <div class="info">
                Current Filters applied:<br />
                <?php
                if ($filters['userId'] != '') {
                    echo "Username :: " . $filters['userId'];
                }
                ?> 
                <br />

                <?php
                if ($filters['dateFrom'] != '-1') {
                    echo "Date From :: " . date('m/d/Y', $filters['dateFrom']);
                }
                ?> 
                <br />
                <?php
                if ($filters['dateTo'] != '-1') {
                    echo "Date To :: " . date('m/d/Y', $filters['dateTo']);
                }
                ?> 
                <br />    
                <form action="/notes/shownotes" method="POST">
                    <input type="hidden" name="username" value="" size="" />

                    <input autocomplete="off" id="date_from" type="hidden" name="date_from" value="" size="50" />
                    <input autocomplete="off" id="date_to" type="hidden" name="date_to" value="" size="50" />

                    <input type="submit" value="Reset" name="submit" />
                </form>  
            </div>    
<?php } ?>

        <div class="innerdiv">              
            <div style="max-height: 400px;overflow: auto;">
                <table id="notes-table" class="bundlelist-table">                   
                    <tr>
                        <th scope="col">User</th>
                        <th scope="col">Date</th>
                        <th scope="col">Message</th>
                        <th scope="col">Report Type</th>
                    </tr>

                    <tbody>
                        <?php if (!empty($data)) { ?>
    <?php foreach ($data as $notes) { ?>
                                <tr>
                                    <td><?php echo $notes->getuserId(); ?></td>
                                    <td><?php echo $notes->getDate(); ?></td>
                                    <td><?php echo $notes->getMessage(); ?></td>
                                    <td><?php echo $notes->getReportType(); ?></td>
                                </tr>
                            <?php } ?>
<?php } else { ?>
                            <tr id="no-data-row">
                                <td colspan="4">No notes available</td>
                            </tr>
<?php } ?>
                    </tbody>
                </table>




            </div>

            <?php
            $number_of_rows = 10;
            $current = $currentPage;
            $total = $totalNotes;
            $pg = paging($current, $number_of_rows, $total, 10);
            ?>

            <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?php echo site_url('notes/shownotes/' . 'rows/' . $number_of_rows . '/page/' . $pg['first']); ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?php echo site_url('notes/shownotes/' . 'rows/' . $number_of_rows . '/page/' . $pg['prev']); ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?php
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $current = 'current'; else
                                $current="";
                            ?>

                            <a href="<?php echo site_url("notes/shownotes/" . "page/$i") ?>" title="Go to Page <?php echo $i; ?>" class="page <?php echo $current; ?>"><span><?php echo $i; ?></span></a>

<? } ?>

                        <a href="<?php echo site_url('notes/shownotes/' . 'rows/' . $number_of_rows . '/page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo site_url('notes/shownotes/' . 'rows/' . $number_of_rows . '/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>

            </div>
        </div>
    </div>
</div>
<script type="text/javascript">
    $(function() {
        var dates = $( "#date_from, #date_to" ).datepicker({
            defaultDate: "+1w",
            changeMonth: true,
            changeYear: true,
            numberOfMonths: 1,
            onSelect: function( selectedDate ) {
                var option = this.id == "date_from" ? "minDate" : "maxDate",
                instance = $( this ).data( "datepicker" ),
                date = $.datepicker.parseDate(
                instance.settings.dateFormat ||
                    $.datepicker._defaults.dateFormat,
                selectedDate, instance.settings );
                dates.not( this ).datepicker( "option", option, date );
            }
        });
        
        // accordian search 
        $("#collapse").addClass("ui-accordion ui-accordion-icons ui-widget ui-helper-reset")
        .find("h3")
        .addClass("ui-accordion-header ui-helper-reset ui-state-default ui-corner-top ui-corner-bottom")
        .hover(function() { $(this).toggleClass("ui-state-hover"); })
        .prepend('<span class="ui-icon ui-icon-triangle-1-e"></span>')
        .click(function() {
            $(this)
            .toggleClass("ui-accordion-header-active ui-state-active ui-state-default ui-corner-bottom")
            .find("> .ui-icon").toggleClass("ui-icon-triangle-1-e ui-icon-triangle-1-s").end()
            .next().slideToggle().toggleClass("ui-accordion-content-active");
            var $link = $('.ui-tabs-nav li:eq(1) a',$(this).next());       
            $link.trigger('click');
            return false;
        })
        .next()
        .addClass("ui-accordion-content  ui-helper-reset ui-widget-content ui-corner-bottom")
        .hide();

        $("#collapse")
        .find("h3 a")
        .focus(function() { $(this).parent().toggleClass("ui-state-hover"); })
        .focusout(function() { $(this).parent().toggleClass("ui-state-hover"); })
        
        
        
    });
</script>
