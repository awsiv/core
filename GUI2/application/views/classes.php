<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>
<div id="tabpane">
    <div class="pagepanel">
        <div class="panelhead">Promises</div>
        <div class="panelcontent">
            <div class="tables">
                <table>
                    <thead>
                        <tr>
                            <th>Host</th>
                            <th>Class context</th>
                            <th>Occurs with probability</th>
                            <th>Uncertainty</th>
                            <th>Last seen</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php foreach ($ret as $value) {  ?>
                            <tr>
                                <td><?php echo $value['host']; ?></td>
                                <td><?php echo $value['context']; ?></td>
                                <td><?php echo $value['prob']; ?></td>
                                <td><?php echo $value['uncert']; ?></td>
                                <td><?php echo date('D F d h:m:s Y', $value['lastseen']);  ?></td>
                            </tr>
                        <?php } ?>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        /*
 $('.tables table').prepend(
     $('<thead></thead>').append($('.tables tr:first').remove())
     );
         */
        $('.tables table').tableFilter();
        $('.tables table').tablesorter({widgets: ['zebra']});
    });
 
</script>


