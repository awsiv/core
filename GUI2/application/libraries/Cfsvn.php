<?

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

//for this class to work we have to install the php svn module support in php engine
function handleError($errno, $errstr, $errfile, $errline) {
    // error was suppressed with the @-operator
    if (0 === error_reporting()) {
        return false;
    }

    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
}

class Cfsvn {

    private $working_dir;
    private $repository;
    private $username;
    private $password;

    function Cfsvn($params) {
          set_error_handler("handleError");
        $this->username = isset($params['username']) ? $params['username'] : NULL;
        $this->password = isset($params['password']) ? $params['password'] : NULL;
        $this->working_dir = isset($params['workingdir']) ? $params['workingdir'] : NULL;
        svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME, $this->username);
        svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD, $this->password);
        svn_auth_set_parameter(SVN_AUTH_PARAM_DONT_STORE_PASSWORDS, true);
        svn_auth_set_parameter(PHP_SVN_AUTH_PARAM_IGNORE_SSL_VERIFY_ERRORS, true);
        svn_auth_set_parameter(SVN_AUTH_PARAM_NO_AUTH_CACHE, true);
        if (!file_exists($this->working_dir) && isset($this->working_dir)) {
            if (!mkdir($this->working_dir, 0700)) {
                echo "cannot create the working directory";
                return;
            }
        }
        $this->repository = isset($params['repository']) ? $params['repository'] : NULL;
    }

    function cfsvn_checkout() {
        try {
        $status = svn_checkout($this->repository, $this->working_dir);
        } catch(Exception $e){
         throw $e;   
        }
        $rev = 0;
        if ($status) {
            //gives an array back with a long list of key value pair for each file in repo
            $repostatus = svn_status($this->working_dir, SVN_ALL);
            for ($i = 0; $i < 1; $i++) {
                $rev = $repostatus[$i]['revision'];
            }
        }
        $data = array(
            'status' => $status,
            'working' => $this->working_dir,
            'rev' => $rev,
            'session' => session_id()
        );
        return $data;
    }

    ///function for commiting a particular file or a whole directory depends upon the working dir parameter
    function cfsvn_commit($comment) {
        $file_stat = svn_status($this->working_dir);
        foreach ($file_stat as $eachfile) {
            if ($eachfile['text_status'] == 2) {
                svn_add($eachfile['path']);
            }
        }

      
        try {
            $cdetails = svn_commit($comment, array($this->working_dir));
            return $cdetails;
        } catch (Exception $e) {
            return array('error' => 'true', 'message' => $e->getMessage());
        }
    }

    function cfsvn_update() {
        $filestat = svn_update($this->working_dir);
        return $filestat;
    }

    function cfsvn_log($limit) {
        $logs = svn_log($this->repository, SVN_REVISION_HEAD, 0, $limit);
        return $logs;
    }

    function cfsvn_working_copy_status() {
        $status = svn_status($this->working_dir, SVN_ALL);
        $modified_files = array();
        foreach ($status as $file) {
            //SVN_WC_STATUS_MODIFIED=8
            if ($file['text_status'] == 8) {
                array_push($modified_files, $file['name']);
            }
        }
        return $modifiled_files;
    }

    function get_current_repository() {
        $status = svn_status($this->working_dir, SVN_ALL);
        $report = "";
        $i = 0;
        for ($i = 0; $i < 1; $i++) {
            $repo = $status[$i]['repos'];
        }
        return $repo;
    }

}