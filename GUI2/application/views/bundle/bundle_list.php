<div class="pagepanel">
    <div class="panelhead">Bundles</div>
    <div class="panelcontent">
        <div class="tables">

            <?php if (!empty($bundle_list['data'])) {
 ?>
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
                    <?php foreach ((array) $bundle_list['data'] as $value) { ?>
                        <tr>
                            <td><?php echo $value['0']; ?></td>
                            <td><a href="<?php echo site_url('/bundle/details'); ?>/bundle/<?php echo $value['1']; ?>"><?php echo $value['1']; ?></a></td>
                            <td><?php echo date('D F d h:m:s Y', $value['2']); ?></td>
                            <td><?php echo $value['3']; ?></td>
                            <td><?php echo $value['4']; ?></td>
                            <td><?php echo $value['5']; ?></td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>
            <?php } else {
 ?>
                    <div>
<?php echo $this->lang->line("no_data"); ?>
                </div>
<?php } ?>
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