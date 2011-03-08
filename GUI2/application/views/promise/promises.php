<div class="pagepanel">
    <div class="panelhead">Promises</div>
    <div class="panelcontent">
        <div class="tables">

            <table>
                <thead>
                    <tr>
                        <th>Host</th>
                        <th>Promise handle</th>
                        <th>Last known state</th>
                        <th>Probability Kept</th>
                        <th>Uncertainty</th>
                        <th>Last seen</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach ((array)$promise_list as $value) {
 ?>
                        <tr>
                            <td><?php echo $value['host']; ?></td>
                            <td><a href="<?php echo site_url() ?>/promise/details/<?php echo $value['handle']; ?>"><?php echo $value['handle']; ?></a></td>
                            <td><?php echo $value['state']; ?></td>
                            <td><?php echo $value['kept']; ?></td>
                            <td><?php echo $value['uncert']; ?></td>
                            <td><?php echo date('D F d h:m:s Y', $value['dt']); ?></td>
                        </tr>
<?php } ?>
                </tbody>
            </table>
        </div>
    </div>
</div>