<?php

class Repository_model extends CI_Model {

    var $collectionName = 'svnrepository';
    var $errors;

    public function __construct() {
        parent::__construct();
        $this->load->library('mongo_db');
        $this->errors = array();
    }

    /**
     * Get all added repository for the user
     * @param <type> $userId 
     */
    function getAllRepository($userId = '') {
        return $this->mongo_db->get($this->collectionName);
    }

    /**
     * Adds a new repository information on the database
     * @param <type> $repoInfo
     * @return bool
     */
    function insertRepository($repoInfo = array()) {

        // validate data
        if (!is_array($repoInfo) && count($repoInfo) == 0) {
            $this->setError('Invalid Parameter passed.');
            return FALSE;
        }
        if ($this->checkDuplicateEntries($repoInfo)) {
            $this->setError('Repository already exists.');
            return FALSE;
        }
        $id = $this->mongo_db->insert($this->collectionName, $repoInfo);
    }

    /**
     * Updates the previous repo information
     * @param array $repoInfo repo info to update
     * @param array $newRepoInfo new info for update
     * @return bool 
     */
    function updateRepository($repoInfo=array(), $newRepoInfo=array()) {
        if (!is_array($repoInfo) && count($repoInfo) == 0) {
            $this->setError('Invalid Parameter passed.');
            return FALSE;
        }

        if (!$this->checkDuplicateEntries($repoInfo)) {
            $this->setError('Cannot find existing repository information to update');
            return FALSE;
        }

        $this->mongo_db->where($repoInfo);
        $this->mongo_db->update($this->collectionName, $newRepoInfo);
        $this->mongo_db->clear();
        return true;
    }

    /**
     * Deletes the specified repository information
     * @param <array> $repoInfo repository to delete
     * @return bool
     */
    function deleteRepository($repoInfo) {
        return $this->mongo_db->where($repoInfo)->delete_all($this->collectionName);
    }

    /**
     * Checks whether there is already existing repo inforamtion for the same user or not
     * @param <array> $repoInfo
     * @return bool
     */
    function checkDuplicateEntries($repoInfo) {

        return $this->mongo_db->where(array('userId' => $repoInfo['userId'], 'repoPath' => $repoInfo['repoPath']))->count($this->collectionName) > 0;
    }

    function hashPassword($userInfo) {

        
        
    }


    function setError($msg) {
        $this->errors[] = $msg;
    }

    function getErrors() {
        return $this->errors;
    }




}

?>
