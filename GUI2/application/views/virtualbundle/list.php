<div class="tables" id="vbundletable">
<div id="infoMessage"><?php echo isset($message) ? $message : ""; ?></div>
<?php
$result = json_decode($report_result, true);
if (count($result['data']) > 0) {
    echo "Total results found: " . $result['meta']['count'];
    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
    echo $this->cf_table->generateVirtualbundleTable($result);
    include APPPATH.'views/searchpages/paging_footer.php';
} else {
    echo"<div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?> 
</div>
