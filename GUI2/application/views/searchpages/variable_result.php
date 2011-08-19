<?php

$result = json_decode($report_result, true);
//$this->table->set_heading(array_keys($result['meta']['header']))
if (key_exists('truncated', $result['meta'])) {
    $message = $result['meta']['truncated'];
    echo "<p class=\"info\">$message</p>";
}
if ($result['meta']['count'] > 0) {
    echo "<h2> Total : " . $result['meta']['count'] . " variables found </h2>";
    foreach ($result as $bundles => $variables) {
        if ($bundles != "meta") {
            echo "<h2> bundle $bundles: <i>" . $variables['count'] . " variables</i></h2>";
            $this->table->set_heading(array_keys($variables['header']));
            foreach ($variables['data'] as $row) {
                //$this->table->add_row($row);
                $temp = array();
                foreach ($variables['header'] as $key => $value) {
                    if (!is_array($value)) {
                        if (strtolower($key) == "lastseen" || strtolower($key) == "time")
                            array_push($temp, date('D F d h:m:s Y', $row[$value]));
                        else
                            array_push($temp, $row[$value]);
                    }
                }
                $this->table->add_row($temp);
            }

            echo $this->table->generate();
        }
        $this->table->clear();
    }
    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
    //echo $report_result .'<br />';
    //echo json_last_error();
    // print_r($result);
    //print_r($heading);
    include 'paging_footer.php';
}
else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>