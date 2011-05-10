<div id="body">
    <div class="outerdiv">
        <div class="innerdiv">
            <div id="notes-filter">
                <div id="stylized">

                    <form action="/notes/shownotes" method="POST">
                        <fieldset>

                        <legend>Search</legend>
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
            <div class="tables" style="overflow:auto; height: 250px;">
                <table id="notes-table">
                    <thead>
                        <tr>
                            <th>User</th>
                            <th>Date</th>
                            <th>Message</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php if (!empty($data)) {
                            ?>
                            <?php foreach ($data['data'] as $index => $d) {
                                ?>
                                <tr>
                                    <td><?php echo $d['user']; ?></td>
                                    <td><?php echo date('D F d h:m:s Y', $d['date']); ?></td>
                                    <td><?php echo $d['message']; ?></td>
                                </tr>
                            <?php } ?>
                        <?php } else {
                            ?>
                            <tr id="no-data-row">
                                <td colspan="3">No new notes</td>
                            </tr>
                        <?php } ?>
                    </tbody>
                </table>
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
    });
</script>
