<?php

class Repository_model extends CI_Model {

    var $collectionName = 'svnrepository';
    var $svn_log_collection_name = 'svnlogs';
    var $approved_policies_collection = 'approvedpolicies';
    var $errors;

    public function __construct() {
        parent::__construct();
        $this->load->library('mongo_db');
        $this->load->helper('date');
        $this->errors = array();
    }

    /**
     * Get all added repository for the user
     * @param <type> $userId : username of the logged in user or system users
     */
    function get_all_repository($userId) {
        if (trim($userId))
            $repos = $this->mongo_db->where(array('userId' => $userId))->get($this->collectionName);
        $this->mongo_db->clear();
        return $repos;
    }

    function get_specific_repository($userId, $repoPath) {
        $this->mongo_db->where(array('userId' => $userId, 'repoPath' => $repoPath))->limit(1);
        $obj = $this->mongo_db->get_object($this->collectionName);
        $this->mongo_db->clear();
        return $obj;
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
        return $id;
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
        if (isset($newRepoInfo['password'])) {
            $newRepoInfo['password'] = $this->encrypt_password($newRepoInfo);
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
        $return = $this->mongo_db->where($repoInfo)->delete_all($this->collectionName);
        $this->mongo_db->clear();
        return $return;
    }

    /**
     * Checks whether there is already existing repo inforamtion for the same user or not
     * @param <array> $repoInfo
     * @return bool
     */
    function check_duplicate_entries($repoInfo) {

        $count = $this->mongo_db->where(array('userId' => $repoInfo['userId'], 'repoPath' => $repoInfo['repoPath']))->count($this->collectionName) > 0;
        $this->mongo_db->clear();
        return $count;
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
        $this->mongo_db->clear();
        return hash('sha256', $obj->password, TRUE);
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

    function decrypt_password($userInfo,$key=NULL) {
        $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
        $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
        $dkey =  is_null($key) ?$this->get_key($userInfo) :$key;
        return trim(
                mcrypt_decrypt(MCRYPT_RIJNDAEL_256,$dkey, base64_decode($userInfo['password']), MCRYPT_MODE_ECB, $iv));
    }

    function set_error($msg) {
        $this->errors[] = $msg;
    }

    function get_errors() {
        return $this->errors;
    }

    /**
     * for tracking user activitiy on various svn repositories
     * @param <type> $username
     * @param <type> $svnrepo
     * @param <type> $file_version
     * @param <type> $operation
     */
    function insert_svn_log($username, $svnrepo, $file_version, $operation) {
        $id = $this->mongo_db->insert($this->svn_log_collection_name, array('username' => $username, 'repo' => $svnrepo, 'version' => $file_version, 'operation' => $operation, 'date' => now()));
    }

    /**
     * get all the activities of all user on repositories
     * @return <type>
     */
    function get_svn_logs($repository='') {
        if ($repository != '') {
            return $this->mongo_db->where(array('repo' => $repository))->get($this->svn_log_collection_name);
        }
        return $this->mongo_db->get($this->svn_log_collection_name);
    }

    /**
     * get unique revision no for a particlular repository
     * @param <type> $repository
     */
    function get_revisions($repository='') {
        $uniqrevs = array();
        $revs = $this->mongo_db->select(array('version'))->where(array('repo' => $repository))->order_by(array('version' => 'desc'))->get($this->svn_log_collection_name);
        foreach ($revs as $rev) {
            array_push($uniqrevs, $rev['version']);
        }
        return array_unique($uniqrevs);
    }

    /**
     * inserting a record for keeping track of accepeted policies on revision number
     * @param <type> $username
     * @param <type> $repo
     * @param <type> $revision
     * @param <type> $comment
     */
    function approve_policies($username, $svnrepo, $revision, $comment) {
        $id = $this->mongo_db->insert($this->approved_policies_collection, array('username' => $username, 'repo' => $svnrepo, 'version' => $revision, 'comment' => $comment, 'date' => now()));
        return $id;
    }

    /**
     * get a list of revision numbers with username and time and comment for depending on the parameters
     * @param <type> $repo 
     */
    function get_all_approved_policies($repo='', $limit='') {
        if ($repo != '' && $limit != '') {
            return $this->mongo_db->where(array('repo' => $repo))->order_by(array('date' => 'desc'))->limit($limit)->get($this->approved_policies_collection);
        } elseif ($limit == '' && $repo != '') {
            return $this->mongo_db->where(array('repo' => $repo))->order_by(array('date' => 'desc'))->get($this->approved_policies_collection);
        } elseif ($repo == '') {
            return $this->mongo_db->order_by(array('date' => 'desc'))->get($this->approved_policies_collection);
        }
    }

    function get_total_approval_count($repo) {
        return $this->mongo_db->where(array('repo' => $repo))->count($this->approved_policies_collection);
    }

}

?>
