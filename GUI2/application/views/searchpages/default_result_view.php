<div class="tables tablesfixed">
<?php
if (is_array($report_result) && key_exists('truncated', $report_result['meta'])) {
    $message = $report_result['meta']['truncated'];
    $displayed_rows = count($report_result['data']);
    echo "<p class=\"info\">$message. Queried for <strong>$number_of_rows rows</strong>, displayed: <strong>$displayed_rows rows</strong>. Please go to next page for more results.</p>";
    $number_of_rows = $displayed_rows;
}

if ($report_result['meta']['count'] > 0) {
    echo '<div class="report_result">';
    echo $this->cf_table->generateReportTableModified($report_result, $report_title);
    echo '</div>';
    include 'paging_footer.php';
} else {
    echo"<div class=\"clearboth\"></div><div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?>
</div>