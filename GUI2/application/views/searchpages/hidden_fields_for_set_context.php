<?php /* hidden fields for set context */   ?>
<input type="hidden" name="host" value="All">
<input type="hidden" name="inclist" value="<?php echo (is_array($paramArray) && isset($paramArray['inclist'])) ? trim($paramArray['inclist']) : ''; ?>">
<input type="hidden" name="exlist" value="<?php echo (is_array($paramArray) && isset($paramArray['exlist'])) ? trim($paramArray['exlist']) : ''; ?>">
<input type="hidden" name="report" value="<?php echo $report_type?>"> 