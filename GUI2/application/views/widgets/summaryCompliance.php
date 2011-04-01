<table>
    <tr>
     <td bgcolor="red">Promise Not Kept <?php echo $startDate; ?> - <?php echo $stopDate; ?></td>
    </tr>
    <tr>
       
        <td>
            <?php
            if (is_array($notkept)) {
                echo $this->cf_table->generateReportTable($notkept);
            } else
                echo "No data found";
            ?>
        </td>
    </tr>

    <tr> <td bgcolor="yellow">Promise Repaired</td></tr>
    <tr>
       

        <td>
            <?php
            if (is_array($repaired)) {
                echo $this->cf_table->generateReportTable($repaired);
            } else
                echo "No data found";
            ?>
        </td>
    </tr>
</table>