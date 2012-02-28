<?php

class class_model extends Cf_Model {
  
    /**
     *
     * @param type $username
     * @param type $classRx
     * @param type $rows
     * @param type $page
     * @return type json, plain array
     */
    function getAllClasses($username, $classRx, $rows=10, $page=1) {
        try {
            $rawdata = cfpr_class_list_distinct_by_name_rx($username, $classRx, $rows, $page);
            $data = $this->checkData($rawdata);
            if (is_array($data)) {
                return json_encode($data['data']);
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
            throw $e;
        }
    }
    
    function getAllTimeClasses($username, $classRx, $rows=10, $page=1){
         try {
            $rawdata = cfpr_class_list_time_distinct_by_name_rx($username, $classRx, $rows, $page);
            $data = $this->checkData($rawdata);
            if (is_array($data)) {
                return json_encode($data['data']);
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
            throw $e;
        }
    }
    
     function getAllSoftClasses($username, $classRx, $rows=10, $page=1){
         try {
            $rawdata = cfpr_class_list_soft_distinct_by_name_rx($username, $classRx, $rows, $page);
            $data = $this->checkData($rawdata);
            if (is_array($data)) {
                return json_encode($data['data']);
            } else {
                throw new Exception($this->lang->line('invalid_json'));
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage()." ".$e->getFile()." line:".$e->getLine());
            throw $e;
        }
    }
}
?>
