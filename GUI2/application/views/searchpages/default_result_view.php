<div class="tables tablesfixed">
<?php
if (is_array($report_result) && key_exists('truncated', $report_result['meta'])) {
    $message = $report_result['meta']['truncated'];
    $displayed_rows = count($report_result['data']);
    echo "<p class=\"info\">$message. Queried for <strong>$number_of_rows rows</strong>, displayed: <strong>$displayed_rows rows</strong>. Please go to next page for more results.</p>";
    $number_of_rows = $displayed_rows;
}

if (count($report_result['data']) > 0) {
    $pg = paging($current, $number_of_rows, $report_result['meta']['count'], 10);
    echo $this->cf_table->generateReportTable($report_result, $report_title);
    include 'paging_footer.php';
} else {
    echo"<div class=\"clearboth\"></div><div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>
</div>