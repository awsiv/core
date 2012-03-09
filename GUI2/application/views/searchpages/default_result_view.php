<div class="tables tablesfixed">
<?php
if(is_array($report_result)){
   $result=$report_result;
}else{
   $result = json_decode($report_result, true);  
}

if (is_array($result) && key_exists('truncated', $result['meta'])) {
    $message = $result['meta']['truncated'];
    $displayed_rows = count($result['data']);
    echo "<p class=\"info\">$message. Queried for <strong>$number_of_rows rows</strong>, displayed: <strong>$displayed_rows rows</strong>. Please go to next page for more results.</p>";
    $number_of_rows = $displayed_rows;
}
$report_title_text=$report_title;
if(isset($hostname) && ($hostname != "")){$report_title_text.=" for " . $hostname;}
echo "<div class=\"grid_9\">".$report_title_text."</div>";
if (count($result['data']) > 0) {
    echo "<div class=\"grid_3\" style=\"text-align: right;\">Total results found: " . $result['meta']['count']."</div> <div class=\"clearboth\"></div>";
    $pg = paging($current, $number_of_rows, $result['meta']['count'], 10);
    echo $this->cf_table->generateReportTable($result, $report_title);
    include 'paging_footer.php';
} else {
    echo"<div class=\"clearboth\"></div><div class='info'>" . $this->lang->line("no_data") . "</div>";
}
?> 
</div>