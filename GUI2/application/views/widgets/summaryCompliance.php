<div style="width:98%;margin:0 auto">
    <table>
    <tr>
     <th scope="red">Promise Not Kept</th>
    </tr>
    <tr>       
        <td id="promise-not-kept" class="bundlelist-table">
            <?php
            if (is_array($notkept)) {
                echo $this->cf_table->generateReportTable($notkept);
            } else
                echo "No data found";
            ?>
        </td>
    </tr>

    <tr> <th scope="yellow">Promise Repaired</th></tr>
    <tr>
        <td id ="promise-repaired" class="bundlelist-table">
            <?php
            if (is_array($repaired)) {
                echo $this->cf_table->generateReportTable($repaired);
            } else
                echo "No data found";
            ?>
        </td>
    </tr>
</table>
</div>