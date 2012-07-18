<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>

<div class="outerdiv grid_12">
    <div class="innerdiv">
        <p class="title">License summary</p>
        <ul>
            <li>Granted <?php echo $licenseGranted ?> licenses.</li>
            <li>Activated on : <?php echo $started; ?></li>
            <li>Expiring on : <?php echo $expiry ?></li>
            <li> Owner :<?php echo $licenseOwner ?></li>
        </ul>
        <br />
        <div class="tables">

            <?php
            foreach ((array) $licenseSummary as $key => $value)
            {
                if ($key == 'lastMeasured')
                {
                    $this->table->add_row($this->lang->line($key), date('D F d h:m:s Y', $value));
                }
                else
                {
                    $this->table->add_row($this->lang->line($key), $value);
                }
            }
            echo $this->table->generate();
            ?>
        </div>
        <div id="licensebar" class="grid_4 push_4">
            <div></div>
        </div>
        <div class="clear"></div>
    </div>
</div>
<script type="text/javascript">
    $('#licensebar').find('div').licensemeter(
    {
        value:<?php echo $pbarvalue ?>,
        daysleft:<?php echo $daysleft ?>
    });
</script>
