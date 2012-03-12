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
      
      
      function getReportTitle($report_id){
        try{
              $rawdata = cfpr_select_reports($report_id);
              $data = $this->checkData($rawdata);
              if (is_array($data) && $this->hasErrors()==0) {
                 return $data['data'][0]['name'];
               throw new Exception($this->lang->line('invalid_report_id')); 
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
              
              }catch (Exception $e){
                  log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
                    throw $e; 
              }
        
      }
      
      function getReportDescription($report_id)
    {
        try
        {
            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0]['description'];
                throw new Exception($this->lang->line('invalid_report_id'));
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
      
      function getReportDetail($report_id)
    {
        try
        {
            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0];
                throw new Exception($this->lang->line('invalid_report_id'));
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
     
      function getBundleReport($username, $hostkey, $search, $inclist, $exlist, $rows=50, $page_number=1)
    {
        try
        {
            $rawdata = cfpr_report_bundlesseen($username, $hostkey, $search, true, $inclist, $exlist, "last-verified", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    function getHostWithBundles($username, $search, $class_regx)
    {

        try
        {
            $rawdata = cfpr_hosts_with_bundlesseen($username, Null, $name, true, $class_regex);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    
    function getClassReport($username, $hostkey, $search, $inclist,$exlist, $rows = 50, $page_number = 1){
        try
        {
            $rawdata = cfpr_report_classes($username, $hostkey, $search, true, $inclist, $exlist,"last-seen", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $days
     * @param type $months
     * @param type $years
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getBusinessValueReport($username, $hostkey, $days, $months, $years, $inclist, $exlist, $rows = 50, $page_number = 1)
    {
         try
        {
            $rawdata = cfpr_report_value($username, $hostkey, $days, $months, $years, $inclist, $exlist, "day", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $days
     * @param type $months
     * @param type $years
     * @param type $class_regex
     * @return type array
     */
    function getHostsWithBusinessValue($username,$days,$months,$years,$class_regex){   
     try
        {
            $rawdata =   cfpr_hosts_with_value($username, NULL, $days, $months, $years, $class_regex);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $state
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getPromiseCompliance($username, $hostkey, $search, $state, $inclist, $exlist, $rows = 50, $page_number = 1)
    {
         try
        {
            $rawdata =   cfpr_report_compliance_promises($username, $hostkey, $search, $state, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getComplianceSummary($username, $hostkey, $inclist, $exlist, $rows = 50, $page_number = 1)
    {
        try
        {
            $rawdata = cfpr_report_compliance_summary($username, $hostkey, NULL, -1, -1, -1, -1, ">", $inclist, $exlist, "last-seen", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $inclist
     * @param type $exlist
     * @param type $longterm_data
     * @param type $rows
     * @param type $page_number
     * @return type 
     */
    function getFileChangeLog($username, $hostkey, $search, $inclist , $exlist, $longterm_data, $rows=50, $page_number=1){
        try
        {   
          if ($longterm_data) {
            $rawdata =   cfpr_report_filechanges_longterm($username, $hostkey, $search, true, -1, ">", $inclist, $exlist, "time", true, $rows, $page_number);
          }else{
            $rawdata =  cfpr_report_filechanges($username, $hostkey, $search, true, -1, ">", $inclist, $exlist, "time", true, $rows, $page_number);
          }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $diff
     * @param type $cal
     * @param type $inclist
     * @param type $exlist
     * @param type $longterm
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getFileChangeDiff($username, $hostkey, $search, $diff, $cal, $inclist, $exlist, $longterm, $rows = 50, $page_number = 1){
       try
        {   
          if ($longterm) {
            $rawdata =   cfpr_report_filediffs_longterm($username, NULL, $search, $diff, true, $cal, ">", $inclist, $exlist, "time", true, $rows, $page_number);
          }else{
            $rawdata =  cfpr_report_filediffs($username, NULL, $search, $diff, true, $cal, ">", $inclist, $exlist, "time", true, $rows, $page_number);
          }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
        
    }
    
    function getLastSeenReport($username, $hostkey, $key, $name, $address, $ago, $inclist, $exlist, $rows = 50, $page_number = 1){
        try
        {   
            $rawdata=cfpr_report_lastseen($username, $hostkey, $key, $name, $address, $ago, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
    function getPatchesAvailable($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 50, $page_number = 1){
      try
        {   
            $rawdata=cfpr_report_patch_avail($username, $hostkey, $search, $version, $arch, true, $inclist, $exlist, "hostname", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->lang->line('invalid_json'));
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
}
?>
