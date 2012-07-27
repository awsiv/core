<div style="width:98%;margin:0 auto">
    <table>
        <tr>
            <th scope="red">Promises not kept</th>
        </tr>
        <tr>       
            <td id="promise-not-kept" class="bundlelist-table">
                <?php
                if (is_array($notkept) && $notkept['meta']['count']>0) {

                    // modify the header and data to add icons 
                    //  var_dump($notkept['meta']['header']);
                    $notkept['meta']['header']['Hosts'] = 3;
                    foreach ($notkept['data'] as &$d) {
                        $link = sprintf('%s/search/index/name/%s/host/All/report/promises-not-kept-summary/hosts_only/true', site_url(), $d[0]);
                        $d[] = sprintf('<a href="%s" target="_self" rel="%s" class="summary-report-host">show</a>', $link, $d[0]);
                    }
                    echo $this->cf_table->generateReportTable($notkept);
                } else
                    echo "No data found";
                ?>
            </td>
        </tr>

        <tr> <th scope="yellow">Promises repaired</th></tr>
        <tr>
            <td id ="promise-repaired" class="bundlelist-table">
                <?php
                if (is_array($repaired) && $repaired['meta']['count']>0) {

                    $repaired['meta']['header']['Hosts'] = 3;
                    foreach ($repaired['data'] as &$r) {
                        $link = sprintf('%s/search/index/name/%s/host/All/report/promises-repaired-summary/hosts_only/true', site_url(), $r[0]);
                        $r[] = sprintf('<a href="%s" target="_self" rel="%s" class="summary-report-host">show</a>', $link, $r[0]);
                    }

                    echo $this->cf_table->generateReportTable($repaired);
                } else
                    echo "No data found";
                ?>
            </td>
        </tr>
    </table>
</div>



