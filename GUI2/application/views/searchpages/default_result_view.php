<div class="tables tablesfixed">
<?php
if (is_array($report_result) && key_exists('truncated', $report_result['meta'])) {
    $message = $report_result['meta']['truncated'];
    $displayed_rows = count($report_result['data']);
    echo "<p class=\"info\">$message. Queried for <strong>$number_of_rows rows</strong>, displayed: <strong>$displayed_rows rows</strong>. Please go to next page for more results.</p>";
    $number_of_rows = $displayed_rows;
}

if (isset($report_result['meta']['old_skipped']) &&
    $report_result['meta']['old_skipped'] > 0) {
    echo "<div class='warning'> Warning: Agents prior to version 2.2.0 are not compatible with this report and are therefore omitted.</div>";
    echo $report_result['meta']['old_skipped'];
}

if ($report_result['meta']['count'] > 0) {
    echo '<div class="report_result">';
    echo $this->cf_table->generateReportTableModified($report_result, $report_title);
    echo '</div>';
    include 'paging_footer.php';
} else {

    // additional info message for patch insallted, available and software report
    $additionalWarningMessage = "";
    $reportTypesArray = array('patches-installed','software-installed','patches-available');
    if (in_array($report_type, $reportTypesArray)) {
       $additionalWarningMessage =  $this->lang->line("if_only_package_promise");
    }

    echo"<div class=\"clearboth\"></div><div class='info'>" . $this->lang->line("no_data")." ".$additionalWarningMessage. "</div>";
}
?>
</div>
