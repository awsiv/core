<?php

$result = json_decode($report_result, true);
if (key_exists('truncated', $result['meta'])) {
    $message = $result['meta']['truncated'];
    echo "<p class=\"info\">$message</p>";
}
$headings = array();
foreach ($result['meta']['header'] as $key => $value) {
    if (strtolower($key) == "change") {
        $column = $key . " Added(+) , Deleted(-) , line No, Content";
        array_push($headings, $column);
    } else {
        array_push($headings, $key);
    }
}
//$this->table->set_heading(array_keys($result['meta']['header']));
$this->table->set_heading($headings);
$heading = "";
if (count($result['data']) > 0) {
    echo "Total results found : " . $result['meta']['count'];
    foreach ($result['data'] as $row) {
        //$this->table->add_row($row);
        $temp = array();
        foreach ($result['meta']['header'] as $key => $value) {
            if (!is_array($value)) {
                if (strtolower($key) == "change detected at")
                    array_push($temp, getDateStatus ($row[$value],true));
                else
                    array_push($temp, $row[$value]);
            }
            else {
                if (strtolower($key) == "change") {
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
    echo $this->table->generate();
    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);

    //$this->table->set_heading($keys);
    //foreach ($result as $key=>$value)
    //{
    //}
    //echo $report_result . '<br />';
    //echo json_last_error();
    // print_r($result);
    //print_r($heading);
    include 'paging_footer.php';
} else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>
