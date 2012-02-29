<?php

class bundle_model extends Cf_Model {
  
    /**
     *
     * @param type $username
     * @return type json, plain array
     */
    function getAllBundles($username) {
        try {
            $rawdata = cfpr_bundle_list_all($username);
            $data = $this->checkData($rawdata);
            if (is_array($data)) {
                return json_encode($data);
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
