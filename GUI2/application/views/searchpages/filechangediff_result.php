<?php
if (key_exists('truncated', $report_result['meta'])) {
    $message = $report_result['meta']['truncated'];
    echo "<p class=\"info\">$message</p>";
}
$headings = array();
foreach ($report_result['meta']['header'] as $key => $value) {
    if (strtolower($key) == "change details") {
        $column = $key . " Added(+) , Deleted(-) , line No, Content";
        array_push($headings, $column);
    } else {
        array_push($headings, $key);
    }
}
$this->table->set_heading($headings);
if (count($report_result['data']) > 0) {
    
   $pg = paging($current, $number_of_rows, $report_result['meta']['count'], 10);
    include 'paging_footer.php';    
    
    foreach ($report_result['data'] as $row) {
        $temp = array();
        foreach ($report_result['meta']['header'] as $key => $value) {
            if (!is_array($value)) {
                if (strtolower($key) == "change detected at")
                    array_push($temp, getDateStatus ($row[$value],true));
                else
                    array_push($temp, $row[$value]);
            }
            else {
                if (strtolower($key) == "change details") {
                    $table = "<table>"; /* generate subkeys for inner table */;
                    $subindexs = array_values($value['subkeys']);
                    $index = $value['index'];

                    foreach ($row[$index] as $subrows) {
                        $table.="<tr>";
                        $minus = false;
                        foreach ($subindexs as $subindex) {
                            $cell = $subrows[$subindex];
                            if ($cell == "-" && $subindex == 0) {
                                $minus = true;
                            }

                            if ($minus && $subindex == 2) {
                                $table.="<td class=\"minus\">$cell</td>";
                            } else {
                                $table.="<td>$cell</td>";
                            }
                        }
                        $table.="</tr>";
                    }
                    $table.="</table>";
                    array_push($temp, $table);
                }
            }
        }
        $this->table->add_row($temp);
    }
    echo '<div class="report_result">'.$this->table->generate().'</div>';
    
    include 'paging_footer.php';
} else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>
