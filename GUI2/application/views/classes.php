<div id="body" class="outerdiv grid_12">
    <div id="tabpane" class="innerdiv">
        <div class="pagepanel">
            <div class="panelhead"><p class="title">Promises</p></div>
            <div class="panelcontent">
                <div class="tables">
                    <?php if (!empty($ret['data'])) {
                    ?>
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
                            <?php foreach ((array) $ret['data'] as $index => $value) {?>
                                <tr>
                                    <td><?php echo $value[0]; ?></td>
                                    <td><?php echo $value[1]; ?></td>
                                    <td><?php echo $value[2]; ?></td>
                                    <td><?php echo $value[3]; ?></td>
                                    <td><?php echo date('D F d h:m:s Y', $value['4']); ?></td>
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
    </div>
</div>
<script type="text/javascript">
    $(document).ready(function() {
        $('.tables table').tablesorter({widgets: ['zebra']});
    });
 
</script>


