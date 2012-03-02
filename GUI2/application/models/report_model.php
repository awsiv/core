<?php
class report_model extends Cf_Model {
    function getAllReports() {
        try {
            $rawdata = cfpr_select_reports(null);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors()==0) {
                return $data;
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
            throw $e;
        }
      }
      
      function getReportCategory($report_id){
          try{
              $rawdata = cfpr_select_reports(null);
              $data = $this->checkData($rawdata);
              if (is_array($data) && $this->hasErrors()==0) {
                  foreach ($data['data'] as $report) {
                    if($report['id'] == $report_id) return $report['category'];
                }
               throw new Exception($this->lang->line('invalid_report_id')); 
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
              
              }catch (Exception $e){
                  log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
                    throw $e; 
              }
          
      }
}
?>
