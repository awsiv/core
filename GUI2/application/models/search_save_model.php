<?php

require_once APPPATH . 'models/Entities/CF_Search.php';

class Search_save_model extends CI_Model {

    var $collectionName = 'custom_search';
    var $errors;

    function __construct() {
        parent::__construct();
    }

    function validate($data) {
        $valid = true;
        if (trim($data['label']) == '') {
            $valid = false;
            $this->setError('Label is empty');
        }

         if (trim($data['url']) == '') {
            $valid = false;
            $this->setError('URL is empty');
        }
        
        if (trim($data['username']) == '') {
            $valid = false;
            $this->setError('username is empty');
        }
        return $valid;
    }

    function insert($data) {

        if (!is_array($data)) {
            $this->setError('Invalid parameter passed to function');
            return false;
        }
        if ($this->validate($data)) {
            $id = $this->mongo_db->insert($this->collectionName, $data);
            if ($id) {
                // insert success
                $searchObj = new CF_Search($data);
                return $searchObj;
            }
        }

        return false;
    }

    function get_all_search($filter = array()) {

        $mongoresult = $this->mongo_db->where($filter)->get($this->collectionName);
        $result = array();
        foreach ((array) $mongoresult as $searchResult) {
            $obj = new CF_Search($searchResult);
            $result[] = $obj;
        }
        return $result;
    }

    function delete($filter) {
        $return = $this->mongo_db->where($filter)->delete_all($this->collectionName);
        $this->mongo_db->clear();
        return $return;
    }

    function setError($errorText) {

        $this->errors[] = $errorText;
    }
    
    function getErrors() {
        return $this->errors;
    }

}

?>
