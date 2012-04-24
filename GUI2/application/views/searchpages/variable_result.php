<?php
if (is_array($report_result['meta']) && key_exists('truncated', $report_result['meta'])) {
    $message = $report_result['meta']['truncated'];
    echo "<p class=\"info\">$message</p>";
}
if ($report_result['meta']['count'] > 0) {
    foreach ($report_result as $bundles => $variables) {
        if ($bundles != "meta") {
            echo "<h2> bundle $bundles: <i>" . $variables['count'] . " variables</i></h2>";
            $this->table->set_heading(array_keys($variables['header']));
            foreach ($variables['data'] as $row) {
                //$this->table->add_row($row);
                $temp = array();
                foreach ($variables['header'] as $key => $value) {
                    if (!is_array($value)) {
                        if (strtolower($key) == "last seen" || strtolower($key) == "time")
                            array_push($temp, getDateStatus($row[$value]));
                        else
                            array_push($temp, $row[$value]);
                    }
                }
                $this->table->add_row($temp);
            }

            echo '<div class="tables tablesfixed report_result">'.$this->table->generate().'</div>';
        }
        $this->table->clear();
    }

    include 'paging_footer.php';
}
else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>