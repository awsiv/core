<?php

class Repository_model extends CI_Model {

    var $collectionName = 'svnrepository';
    var $svn_log_collection_name='svnlogs';
    var $errors;

    public function __construct() {
        parent::__construct();
        $this->load->library('mongo_db');
        $this->ci->load->helper('date');
        $this->errors = array();
    }

    /**
     * Get all added repository for the user
     * @param <type> $userId 
     */
    function get_all_repository($userId = '') {

        if (trim($userId)) $this->mongo_db->where(array('username'=>$userId));
        return $this->mongo_db->get($this->collectionName);
    }

    /**
     * Adds a new repository information on the database
     * @param <type> $repoInfo
     * @return bool
     */
    function insert_repository($repoInfo = array()) {


        // validate data
        if (!is_array($repoInfo) && count($repoInfo) == 0) {
            $this->setError('Invalid Parameter passed.');
            return FALSE;
        }
        if ($this->check_duplicate_entries($repoInfo)) {
            $this->set_error('Repository already exists.');
            return FALSE;
        }

        $repoInfo['password'] = $this->encrypt_password($repoInfo);

        $id = $this->mongo_db->insert($this->collectionName, $repoInfo);
    }

    /**
     * Updates the previous repo information
     * @param array $repoInfo repo info to update
     * @param array $newRepoInfo new info for update
     * @return bool 
     */
    function update_repository($repoInfo=array(), $newRepoInfo=array()) {
        if (!is_array($repoInfo) && count($repoInfo) == 0) {
            $this->set_error('Invalid Parameter passed.');
            return FALSE;
        }

        if (!$this->check_duplicate_entries($repoInfo)) {
            $this->set_error('Cannot find existing repository information to update');
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
    function delete_repository($repoInfo = array()) {
        return $this->mongo_db->where($repoInfo)->delete_all($this->collectionName);
    }

    /**
     * Checks whether there is already existing repo inforamtion for the same user or not
     * @param <array> $repoInfo
     * @return bool
     */
    function check_duplicate_entries($repoInfo) {

        return $this->mongo_db->where(array('userId' => $repoInfo['userId'], 'repoPath' => $repoInfo['repoPath']))->count($this->collectionName) > 0;
    }

    /**
     * get key for Encryption
     * @param <type> $userInfo 
     */
    function get_key($userInfo) {
        $obj = $this->mongo_db->select(array('username', 'id', 'password', 'group'))
                        ->where(array('username' => $userInfo['userId'], 'active' => 1))
                        ->limit(1)
                        ->get_object('users');

        return hash('sha256', $obj->_id, TRUE);
    }

    /**
     * Hash the passowrd
     * @param <type> $userInfo 
     */
    function encrypt_password($userInfo) {
        $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
        $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
        $key = $this->get_key($userInfo);
        $text = $userInfo['password'];
        $cryptpass = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, $text, MCRYPT_MODE_ECB, $iv);
        return base64_encode($cryptpass);
    }

    function decrypt_password($userInfo) {
        $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
        $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
        return trim(
                mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $this->get_key($userInfo), base64_decode($userInfo['password']),
                        MCRYPT_MODE_ECB,
                        $iv));
    }

    function set_error($msg) {
        $this->errors[] = $msg;
    }

    function get_errors() {
        return $this->errors;
    }
    /**
     *for tracking user activitiy on various svn repositories
     * @param <type> $username
     * @param <type> $svnrepo
     * @param <type> $file_version
     * @param <type> $operation
     */
    function insert_svn_log($username,$svnrepo,$file_version,$operation)
    {
      $id=$this->mongo_db->insert($this->svn_log_collection_name,array('username'=>$username,'repo'=>$svnrepo,'version'=>$file_version,'date'=>now()));
    }



    /**
     *get all the activities of all user on repositories
     * @return <type>
     */
    function get_svn_logs()
    {
       return $this->mongo_db->get($this->svn_log_collection_name);
    }


    
   /**
    *get unique revision no for a particlular repository
    * @param <type> $repository
    */
    function get_revisions($repository='')
    {
        $revs=$this->mongo_db->select(array('version'))->where(array('repo'=>$repository))->get($this->svn_log_collection_name);
        return array_unique($revs);
    }

}

?>
