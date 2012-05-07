    <div class="outerdiv">
        <div class="innerdiv">
            <div id="collapse">
                <h3><a href="#">Search</a></h3>
                <div id="notes-filter" style="padding:5px;">
                    <div class="stylized">
                        <form action="<?php echo site_url(); ?>/notes/shownotes" method="POST">
                            <fieldset>
                                <label id="Username" for="userId">User name: </label>
                                <input type="text" name="userId" value="<?php echo $filters['userId']; ?>" size="50" />

                                <label for="dateFrom">Date from: </label>
                                <input autocomplete="off" id="dateFrom" type="text" name="dateFrom" value="<?php echo $display_dateFrom; ?>" size="50" />

                                <label for="dateTo">Date to: </label>
                                <input autocomplete="off" id="dateTo" type="text" name="dateTo" value="<?php echo $display_dateTo; ?>" size="50" />
                                <label for="submit"></label>
                                <input type="submit" value="search" name="submit" />
                            </fieldset>

                            <br />
                        </form>
                    </div>
                </div>
            </div>
             <?php if (!empty($data)) { ?>
            <div style="max-height: 400px;overflow: auto;">
                <table id="notes-table" class="bundlelist-table">
                    <tr>
                        <th scope="col">User</th>
                        <th scope="col">Date</th>
                        <th scope="col">Message</th>
                        <th scope="col">Report Type</th>
                        <th scope="col"></th>
                    </tr>

                    <tbody>

                            <?php foreach ($data as $notes) { ?>
                                <tr>
                                    <td><?php echo $notes->getuserId(); ?></td>
                                    <td><?php echo $notes->getDate(); ?></td>
                                    <td><?php echo $notes->getMessage(); ?></td>
                                    <td><?php echo $notes->getReportType(); ?></td>
                                    <td>
                                        <a class="details" href="#">Details</a>
                                        <div class="detailsTables" style="display: none;">
                                            <table class="bundlelist-table">
                                                <thead>
                                                    <tr>
                                                        <th scope="col">Hostname</th>
                                                        <th scope="col">IP</th>
                                                        <?php if ($notes->getReportType() != 'Hosts') { ?>
                                                        <?php foreach ($notes->getReport() as $repKey => $rep) { ?>
                                                            <th scope="col">
                                                                <?php echo $repKey ?>
                                                            </th>
                                                        <?php }} ?>
                                                    </tr>
                                                </thead>
                                                <tbody>
                                                    <tr>
                                                        <?php $hostArray = $notes->getHost(); ?>
                                                        <td><a href="<?php echo site_url();?>/welcome/host/<?php echo $hostArray['kh'];?>"><?php echo $hostArray['name']; ?></a></td>
                                                        <td><?php echo $hostArray['ip']; ?></td>
                                                         <?php if ($notes->getReportType() != 'Hosts') { ?>
                                                        <?php foreach ($notes->getReport() as $repKey => $rep) { ?>
                                                            <td>
                                                                <?php echo $rep ?>
                                                            </td>
                                                        <?php }} ?>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </div>
                                    </td>
                                </tr>
                            <?php } ?>
                    </tbody>
                </table>




            </div>

            <?php
            $current = $currentPage;
            $total = $totalNotes;
            $pg = paging($current, $number_of_rows, $total, 10);
            ?>

            <div class="Paging">
                <div class="pages">
                    <div class="inside">
                        <a href="<?php echo site_url('notes/shownotes' .$pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['first']); ?>" title="Go to First Page" class="first"><span>First</span></a>
                        <a href="<?php echo site_url('notes/shownotes' .$pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['prev']); ?>" title="Go to Previous Page" class="prev"><span><</span></a>

                        <?php
                        for ($i = $pg['start']; $i <= $pg['end']; $i++) {
                            if ($i == $pg['page'])
                                $current = 'current'; else
                                $current = "";
                            ?>

                            <a href="<?php echo site_url("notes/shownotes".$pagingParam. "/rows/" . $number_of_rows."/page/$i") ?>" title="Go to Page <?php echo $i; ?>" class="page <?php echo $current; ?>"><span><?php echo $i; ?></span></a>

                        <?php } ?>

                        <a href="<?php echo site_url('notes/shownotes' . $pagingParam.'/rows/' . $number_of_rows . '/page/' . $pg['next']) ?>" title="Go to Next Page" class="next"><span>></span></a>
                        <a href="<?php echo site_url('notes/shownotes' . $pagingParam. '/rows/' . $number_of_rows . '/page/' . $pg['last']) ?>" title="Go to Last Page" class="last"><span>Last</span></a>
                    </div>
                </div>

            </div>
             <?php } else { ?>
                     <div class="info">No notes available</div>
             <?php } ?>
        </div>

    <div id="detailsDialog"></div>
    </div>
<script type="text/javascript">
    $(function() {
        var dates = $( "#dateFrom, #dateTo" ).datepicker({
            defaultDate: "+1w",
            changeMonth: true,
            changeYear: true,
            numberOfMonths: 1,
            onSelect: function( selectedDate ) {
                var option = this.id == "dateFrom" ? "minDate" : "maxDate",
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
        
        
        
            var $detailsDialog = $('#detailsDialog').dialog({
		 autoOpen: false,
		 modal: true,
                 width: 'auto',
                 title: 'Note details',
		 buttons: {
		 'Ok': function() {
	         $(this).dialog('close');
	         }
		 },
		 open: function() {
		 }
	 });
         
         $('.details').bind('click',function(event){
             event.preventDefault();
             var content = $(this).next().html();
             $('#detailsDialog').html(content);
             $detailsDialog.dialog('open');
         
         });
        
        
        
        
    });
</script>
