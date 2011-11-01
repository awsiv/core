<div class="tables tablesfixed">
<?php
$result = json_decode($report_result, true);
if (is_array($result) && key_exists('truncated', $result['meta'])) {
    $message = $result['meta']['truncated'];
    $displayed_rows = count($result['data']);
    echo "<p class=\"info\">$message. Queried for <strong>$number_of_rows rows</strong>, displayed: <strong>$displayed_rows rows</strong>. Please go to next page for more results.</p>";
    $number_of_rows = $displayed_rows;
}
if (count($result['data']) > 0) {
    echo "Total results found: " . $result['meta']['count'];
    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
    echo $this->cf_table->generateReportTable($result, $report_title);
    include 'paging_footer.php';
} else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?> 
</div>