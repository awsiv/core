<div class="pagepanel">
    <div class="panelhead">Bundles</div>
    <div class="panelcontent">
        <div class="tables">


            <table>
                <thead>
                    <tr>
                        <th>Host</th>
                        <th>Bundle list</th>
                        <th>Last verified</th>
                        <th>Hours ago</th>
                        <th>Avg. interval</th>
                        <th>Uncertainty</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach ((array) $bundle_list as $value) {
                    ?>
                        <tr>
                            <td><?php echo $value['host']; ?></td>
                            <td><a href="<?php echo site_url('/bundle/details'); ?>/bundle/<?php echo $value['bundle']; ?>"><?php echo $value['bundle']; ?></a></td>
                            <td><?php echo date('D F d h:m:s Y', $value['verified']); ?></td>
                            <td><?php echo $value['ago']; ?></td>
                            <td><?php echo $value['avg']; ?></td>
                            <td><?php echo $value['uncert']; ?></td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
</div>
<script type="text/javascript">

    /*$(document).ready(function() {
        $('.tables table').prepend(
        $('<thead></thead>').append($('.tables tr:first').remove())
    );
     */
    $('.tables table').tableFilter();
    $('.tables table').tablesorter({widgets: ['zebra']});
	
    
</script>