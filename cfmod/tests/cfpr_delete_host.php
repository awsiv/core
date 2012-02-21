<?php
$userName = getenv("CFENGINE_TEST_USERNAME");

# create dummy then delete, this is hackish but we can't delete real data
$hostKeyDummy = "DUMMYHOSTKEY";

exec("/var/cfengine/bin/mongo cfreport --eval \"db.hosts.update({kH:'$hostKeyDummy'},{kH:'$hostKeyDummy'},true)\"");

cfpr_delete_host($userName, $hostKeyDummy);

?>
