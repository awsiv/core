<div id="body">
    <div class="outerdiv">
        <div class="innerdiv">              
            <?php if (isset($replicationStatusData) && !empty($replicationStatusData)) { ?>
                <p class="title">Replica Name: <b> <?php echo $replicationStatusData['set']; ?></b>

                    <?php if (!$replicationStatusData['status']) { ?>
                <div style="margin:5px;width: 95%">
                        <p class="error">Some members of the replication set are failing. </p>
                    </div>
                <?php } ?>

                <?php foreach ((array) $replicationStatusData['members'] as $host) { ?>
                    <?php $scope = ($host['health']) ? 'green' : 'red'; ?>
                    <table class="bundlelist-table">
                        <thead>
                            <tr><th scope="<?php echo $scope; ?>" colspan="4"><?php echo $host['name']; ?></th></tr>
                            <tr>
                                <th scope="col">Health</th>
                                <th scope="col">Uptime</th>
                                <th scope="col">State</th>
                                <th scope="col">Last heartbeat</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td><?php echo $host['health'] ? 'OK' : 'Failing'; ?></td>
                                <td><?php echo isset($host['uptime']) ? formatSeconds($host['uptime']) : '-'; ?>  </td>
                                <td><?php echo isset($host['stateStr']) ? $host['stateStr'] : '-'; ?></td>
                                <td><?php echo isset($host['lastHeartbeat']) ? getDateStatus($host['lastHeartbeat'], true) : '-'; ?></td>
                            </tr>
                        </tbody>
                    </table>


                <?php } ?>  

            <?php } else { ?>
                <div class="info"> No replica set configured. </div>
            <?php } ?>    
        </div>
    </div>
</div>
