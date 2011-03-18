<div class="pagepanel">
    <div class="panelhead">Promises</div>
    <div class="panelcontent">
        <div class="tables">
            <?php if (!empty($promise_list['data'])) {
            ?>
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
                    <?php foreach ((array) $promise_list['data'] as $value) {
                    ?>
                        <tr>
                            <td><?php echo $value['0']; ?></td>
                            <td><a href="<?php echo site_url() ?>/promise/details/<?php echo $value['1']; ?>"><?php echo $value['1']; ?></a></td>
                            <td><?php echo $value['2']; ?></td>
                            <td><?php echo $value['3']; ?></td>
                            <td><?php echo $value['4']; ?></td>
                            <td><?php echo date('D F d h:m:s Y', $value['5']); ?></td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>
            <?php } else { ?>
                    <div>
                <?php echo $this->lang->line("no_data"); ?>
                </div>
            <?php } ?>
        </div>
    </div>
</div>