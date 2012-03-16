<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

/**
 * Uses Nova Ldap module to perform Ldap queries
 */
class Auth_Ldap {

    protected $errors = array();
    protected $error_start_delimiter;
    protected $error_end_delimiter;
    protected $renamedfields = array();
    protected $host;
    protected $basedn;
    protected $mode;
    protected $login_attribute;
    protected $users_directory;
    protected $member_attribute;
    protected $ad_domain;
    protected $encryption;
    protected $cause= array();
    protected $warnings=array();
    protected $warning_cause=array();
    protected $authenticated=false;
    protected $user_directories=array();
    protected $user_dn;

    function __construct() {
        $this->ci = & get_instance();
        log_message('debug', 'Auth_Ldap initialization commencing...');

        // Load the session library
        $this->ci->load->library(array('session', 'ion_auth'));
        $this->ci->load->config('ion_auth', TRUE);
        $this->ci->load->model('settings_model');
        // Load the configuration
        //$this->ci->load->config('auth_ldap');
        $this->error_start_delimiter = $this->ci->config->item('error_start_delimiter', 'ion_auth');
        $this->error_end_delimiter = $this->ci->config->item('error_end_delimiter', 'ion_auth');
        $this->warning_start_delimiter='<p class="warning">';
        $this->warning_end_delimiter='</p>';
        // Load the language file
         $this->ci->lang->load('ion_auth');

        $this->_init();
    }

    /**
     * @access private
     * @return void
     */
    private function _init() {

        // Verify that the LDAP extension has been loaded/built-in
        // No sense continuing if we can't

        /* if (!function_exists('ldap_connect')) {
          show_error('LDAP functionality not present.  Either load the module ldap php module or use a php with ldap support compiled in.');
          log_message('error', 'LDAP functionality not present in php.');
          } */

        if (!function_exists('cfpr_ldap_authenticate')) {
            show_error('LDAP functionality not present. Ldap support module in Nova is not loaded.');
            log_message('error', 'LDAP functionality not present in php.');
        }

        $appsettings = $this->ci->settings_model->get_app_settings();
        if (!$appsettings) {
            $this->_audit("Error loading application setting.Check mongodb");
            $this->set_error("error_loading_application_setting");
            return false;
        }
       
        $this->hosts = $appsettings->host;
        $this->basedn = trim($appsettings->base_dn);
        $this->login_attribute = trim($appsettings->login_attribute);

        $this->mode = $appsettings->mode;
        if ($this->mode == 'ldap') {
            $this->use_ad = false;
        } elseif ($this->mode == 'active_directory') {
            $this->use_ad = true;
        } else {
            $this->use_ad = false;
        }

        $this->ad_domain = $appsettings->active_directory_domain;
        $this->member_attribute = trim($appsettings->member_attribute);

        if ($this->use_ad && $this->login_attribute == '') {
            $this->login_attribute = 'sAMAccountName';
        } elseif (!$this->use_ad && $this->login_attribute == '') {
            $this->login_attribute = 'uid';
        }
        
        $this->user_directories=explode(';',$appsettings->users_directory);
        if(count($this->user_directories)==1){
            $this->users_directory= $this->user_directories[0];
        }
       
        $this->renamedfields = array(strtolower($this->login_attribute) => 'name', 'mail' => 'email', 'cn' => 'displayname', 'givenname' => 'name');
       
        /**
         * Deciding what encryption method is used
         */
         $this->use_ssl=false;
         $this->use_starttls=false;
        (isset($appsettings->encryption))?$this->encryption=$appsettings->encryption:$this->encryption='plain'; 
         if($this->encryption=='ssl'){
                $this->use_ssl=true;
            }elseif($this->encryption=='start_tls'){
               $this->use_starttls=true;
            }
       
        /**
         * for creating ldap connection string depending on the value
         */
        if($this->use_ssl){
            $this->ldap_url = "ldaps://" . $this->hosts;
        }else{
           $this->ldap_url = "ldap://" . $this->hosts;
        }

        //$this//->proxy_user = property_exists($appsettings, 'proxy_user') ? $appsettings->proxy_user : "";
        // $this->proxy_pass = $this->ci->config->item('proxy_pass');
        //$this->proxy_user = property_exists($appsettings, 'proxy_user') ? $appsettings->proxy_user : "";
        //$this->roles = $this->ci->config->item('roles');
        //$this->auditlog = $this->ci->config->item('auditlog');
    }

    /**
     * @access public
     * @param string $username
     * @param string $password
     * @return array
     */
    function login($username, $password) {
        
        //will have an array or return false incase of invalid authentication
        $user_info = $this->_authenticate($username, $password);
        /* For future use to search in a valid role
         * if (empty($user_info['role'])) {
          log_message('info', $username." has no role to play.");
          show_error($username.' succssfully authenticated, but is not allowed because the username was not found in an allowed access group.');
          return FALSE;
          } */
        if (is_array($user_info)) {
            // Record the login  // Set the session data
            $this->_audit("Successful login: " . $user_info['cn'] . "(" . $username . ") from " . $this->ci->input->ip_address());
            $customdata = array(
                'user_id' => $username,
                'username' => $username,
                'cn' => $user_info['cn'],
                'role' => $user_info['role'],
                'dn'=>$user_info['dn']
            );
            return $customdata;
        }
        return FALSE;
    }

    /**
     * @access public
     * @return bool
     */
    function is_authenticated() {
        if ($this->ci->session->userdata('logged_in')) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    /**
     * @access public
     */
    function logout() {
        // Just set logged_in to FALSE and then destroy everything for good measure
        $this->ci->session->set_userdata(array('logged_in' => FALSE));
        $this->ci->session->sess_destroy();
    }

    /**
     * @access private
     * @param string $msg
     * @return bool
     */
    private function _audit($msg) {
        $date = date('Y/m/d H:i:s');
        /* if( ! file_put_contents($this->auditlog, $date.": ".$msg."\n",FILE_APPEND)) {
          log_message('info', 'Error opening audit log '.$this->auditlog);
          return FALSE;
          } */
        log_message('info', '$date  ' . $msg);
        return TRUE;
    }

    /**
     * @access private
     * @param string $username
     * @param string $password
     * @return array 
     */
    private function _authenticate($username, $password) {
        $needed_attrs = array('dn', 'cn', $this->login_attribute);
        // We've connected, now we can attempt the login.
        if ($this->use_ad) {
            $entries = "";
            $details = array();
            //set up adconnection giving username and password;
            if ($this->_set_up_ADconnection($username, $password)) {
                $details = $this->get_details_for_user($username, $password);
                if (empty($details)) {
                    $this->set_error('no_entries_found');
                    log_message('error', 'Unable to find any entry may be due to in correct user directory or login attribute value');
                    return False;
                }
            } else {
                return False;
            }
            $cn = $details[0]['displayname'];
            $dn = $details[0]['distinguishedname'];
            $id = $details[0]['name'];
        } else {

            // Now actually try to bind as the user using default binddn if anom bind possible grab the dn form ldap
            try{
                $bind=false;
                foreach($this->user_directories as $directory){
                     $binddn = $this->login_attribute . '=' . $username . ',' . $directory . ',' . $this->basedn;
                     $bind = cfpr_ldap_authenticate($this->ldap_url, $binddn, $password,$this->use_starttls);
                     if($bind){
                         $this->users_directory=$directory;
                         break;
                     }    
                }
                
              }
            catch(Exception $e){
              $this->set_error('ldap_conf_error', $e->getMessage());
              if($e->getMessage()=='Invalid credentials'){
                   log_message('error', 'Unable to perform LDAP bind due to invalid values either in user directory field or username and password, '.$e->getMessage().'  ' .$e->getLine());
              }elseif($e->getMessage()=='Server is unavailable'){
                   log_message('error', 'Unable to perform LDAP bind ,make sure the correct encryption is set and valid values supplied in hostname, '.$e->getMessage().'  ' .$e->getLine());
              }else{
                   log_message('error', 'Unable to perform LDAP bind, '.$e->getMessage().'  ' .$e->getLine());
              }
              return FALSE;
            }
                if (!$bind) {
                    log_message('error', 'Unable to perform LDAP bind, Invalid credentials or wrong user directory name');
                    $this->set_error('ldap_login_error','Invalid credentials or wrong user directory name');
                    return FALSE;
                } else {
                    $details = $this->get_details_for_user($username, $password);
                    if (count($details) == 0) {
                        log_message('error', 'Unable to find any entry may be due to in correct user directory or login attribute value');
                        $this->set_error('no_entries_found');
                        return False;
                    }
                }
            $gid=null;    
            $cn = key_exists('displayname', $details[0])?$details[0]['displayname']:"";
            $dn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $id = key_exists('name', $details[0])?$details[0]['name']:"";
            //$grp_attr=explode(',',$this->member_attribute);
            $gid= key_exists('gidNumber', $details[0])?$details[0][strtolower('gidNumber')]:""; 
        }
      
        $this->authenticated=true;
        $roles = $this->get_role_for_user_from_localdb($username);
        /*if ($this->use_ad) {
           $roles = $this->get_role_for_user($username, $password, $dn);
            
        } else {
            $roles = $this->get_role_for_user($id, $password,null,$gid);
        }*/
        return array('cn' => $cn, 'dn' => $dn, 'id' => $id,
            'role' => $roles); 
    }

    /**
     * @access private
     * @param string $str
     * @param bool $for_dn
     * @return string 
     */
    private function ldap_escape($str, $for_dn = false) {
        /**
         * This function courtesy of douglass_davis at earthlink dot net
         * Posted in comments at
         * http://php.net/manual/en/function.ldap-search.php on 2009/04/08
         */
        // see:
        // RFC2254
        // http://msdn.microsoft.com/en-us/library/ms675768(VS.85).aspx
        // http://www-03.ibm.com/systems/i/software/ldap/underdn.html  

        if ($for_dn)
            $metaChars = array(',', '=', '+', '<', '>', ';', '\\', '"', '#');
        else
            $metaChars = array('*', '(', ')', '\\', chr(0));

        $quotedMetaChars = array();
        foreach ($metaChars as $key => $value)
            $quotedMetaChars[$key] = '\\' . str_pad(dechex(ord($value)), 2, '0');
        $str = str_replace($metaChars, $quotedMetaChars, $str); //replace them
        return ($str);
    }

   
    private function _set_up_ADconnection($username, $password) {
        if (preg_match('/^(\w+\.)+\w{2,5}$/', $this->ad_domain)) {
            $binddn = $username . '@' . $this->ad_domain;
        } else {
            $binddn = $this->ad_domain . '\\' . $username;
        }
        
        try{
                $bind = cfpr_ldap_authenticate($this->ldap_url, $binddn, $password,$this->use_starttls);
            }
            catch(Exception $e){
              $this->set_error('active_dir_conf_error',$e->getMessage());
              if($e->getMessage()=='Invalid credentials'){
                 log_message('error', 'Unable to perform Active directory bind due to invalid values either in user directory field or username and password, '.$e->getMessage().'  ' .$e->getLine());
              }elseif($e->getMessage()=='Server is unavailable'){
                 log_message('error', 'Unable to perform Active directory bind ,make sure the correct encryption is set and valid values supplied in hostname, '.$e->getMessage().'  ' .$e->getLine());
              }else{
                 log_message('error', 'Unable to perform Active directory bind, '.$e->getMessage().'  ' .$e->getLine());
              }
              return FALSE;
          }
        //$bind = @ldap_bind($this->ldapconn, $binddn, $password);
        if (!$bind) {
             $this->set_error('active_dir_login_error');
            log_message('error', 'Unable to perform Active directory bind, Invalid credentials');
            return FALSE;
        }
        return TRUE;
    }

    /**
     *
     * @param <type> $username
     * @param <type> $password
     * @return <type> The array of associative arrays having keys username display name and email
     */
    function get_all_ldap_users($username=false, $password=false) {
        
        if ($this->use_ad) {
            $filter = "(&(objectCategory=person)(objectClass=user)(cn=*)(sAMAccountName=*))";
            $fields = "$this->login_attribute,cn,mail";
            $dn = $this->users_directory==''?$this->basedn:$this->users_directory . ',' . $this->basedn;;
            if (preg_match('/^(\w+\.)+\w{2,5}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            //return $this->search_ldap($username, $password, $filter, $fields);
            $result = $this->cfpr_ldap_search($binddn, $password, $filter, $fields, $dn);
            return $result;
        } else {
            $filter = "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson)(mail=*))";
            $fields = $this->login_attribute . ",cn,mail";
            $collectedusers=array();
            foreach($this->user_directories as $directory){
                $dn = $directory . ',' . $this->basedn;
                if($this->user_dn!=''){
                  $userdn = $this->user_dn;
                }else{
                  $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;   
                }
                $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
                if(!is_null($result)){
                 $collectedusers=array_merge($collectedusers,$result);
                 
                }
            }
            return $collectedusers;
        }
    }

    /**
     *
     * @param <type> $username
     * @param <type> $password
     * @return <type> Returns array of associative array each conaining different attributes mentioned in the $fields array;
     */
    function get_all_ldap_roles($username, $password) {
        if ($this->use_ad) {
            $filter = '(&(objectCategory=group))';
            $fields = "sAMAccountName";
            $dn = $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,5}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_search($binddn, $password, $filter, $fields, $dn);
            return $result;
        } else {
            if($this->member_attribute!='' && strtolower($this->member_attribute) !='gidnumber'){
            $filter = '(|(objectClass=posixGroup)(' . $this->member_attribute . '=*))';
            }else{
            $filter = '(|(objectClass=posixGroup))';   
            }
            $fields = "cn";
            $dn = $this->basedn;
              if($this->user_dn!=''){
                  $userdn = $this->user_dn;   
                }else{
                  $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;   
                }
            $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
            return $result;
        }
    }
     
    /**
     *
     * @param type $username
     * @param type $password
     * @param type $get_role_arg
     * @param type $gid
     * @return Returns a the roles for the user form ldap .
     */
    function get_role_for_user($username, $password, $get_role_arg=null,$gid=null) {
        $result=array();
        if ($this->use_ad) {
            $escaped = $this->ldap_escape($get_role_arg, false);
            $filter = '(member=' . $escaped . ')';
            $field = "cn";
            $dn = $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,5}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_single_search($binddn, $password, $filter, $field, $dn);
        } else {
            $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
                if($this->member_attribute!=''){
                $filter = '(|(&(gidNumber='.$gid.')(objectClass=posixGroup))(' .$this->member_attribute. '=' . $username . '))';
                }else{
                $filter = '(&(gidNumber='.$gid.')(objectClass=posixGroup))';   
                }
            $field = "cn";
            $dn = $this->basedn;
            $result = $this->cfpr_ldap_single_search($userdn, $password, $filter, $field, $dn);
        }
         if(empty($result)){
                $available_groups=$this->get_all_ldap_roles($username,$password);
                if(!empty($available_groups) && $this->authenticated){
                   $this->set_warning('no_roles_for_user'); 
                   log_message('error', 'User does not belong to any group, But can login into mission portal with limited access');
                }elseif($this->authenticated){
                   $this->set_warning('no roles fetched'); 
                   log_message('error', 'No groups could be fetched from ldap'); 
                }else{
                $this->set_warning('error_fetching_role', 'invalid member attribute or user attribute');
                log_message('error', 'Error fetching groups from directory service  possibly due to invalid member attribute or user directory  value');
                } 
            }
        return $result;
    }

     /**
      *
      * @param type $username
      * @param type $password
      * @return Returns the details for users like the username , unique dn , etc
      */
    function get_details_for_user($username, $password) {
        $result=array();
        $filter = '(' . $this->login_attribute . '=' . $username . ')';
        if ($this->use_ad) {
            $fields = "givenName,cn,distinguishedName";
            $dn = $this->users_directory==''?$this->basedn:$this->users_directory . ',' . $this->basedn;
            
            if (preg_match('/^(\w+\.)+\w{2,5}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_search($binddn, $password, $filter, $fields, $dn); 
        } else {
            //$groups=explode(',',  $this->member_attribute);
            $fields = "cn,dn,uid,gidNumber";
            $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $dn = $this->users_directory . ',' . $this->basedn;
            $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
        }
        return $result;
    }
    
    /**
     *
     * @param type $username
     * @returns the roles for the ldap user from local Db as matched by the mission portal
     */
    function get_role_for_user_from_localdb($username){
        $roles=array();
        $user=$this->ci->ion_auth->get_ldap_user_details_from_local_db($username);
        if($user !==NULL && is_object($user)){
        $roles=$user->roles;
        }
        return $roles;
    }
    
    
    /**
     *
     * @param type $user_dn
     * @param type $password
     * @param type $filter
     * @param type $fields
     * @param type $dn
     * @return array  helper funtion that utilizes the cfpr ldap funtion form the web api in noval module to support 
     * upper function
     */
    function cfpr_ldap_search($user_dn, $password, $filter, $fields, $dn) {
        try{
             $result = cfpr_ldap_get_several_attributes($this->ldap_url,
                        $user_dn,
                        $dn,
                        $filter,
                        $fields,
                        "subtree",
                        "sasl",
                        $password, 1, 100,$this->use_starttls);
           }catch(Exception $e){
           $this->set_error('ldap_value_grabbing_error',$e->getMessage());
           log_message('error', 'Error grapping value from directory service, '.$e->getMessage().'  '.$e->getLine() );
           return array();
         }
        $temp_array = explode(',', $fields);
        foreach($temp_array as $val){
            $fields_array[]=strtolower($val);
        }
        $ret = array();
        $users = json_decode($result, true);
        if (!is_array($users)) {
            log_message('error', 'Wrong data format returned by ldap module');
            //$this->set_error("Internal_Ldap_module_error");
            return array();
        }
        foreach ($users['data'] as $user) {
            $row = array();
            foreach ($users['keys'] as $key => $value) {
                if (key_exists(strtolower($key), $this->renamedfields)) {
                    $row[$this->renamedfields[strtolower($key)]] = (array_search(strtolower($key), $fields_array) !== FALSE) ? $user[$value] : "";
                } else {
                    $row[strtolower($key)] = (array_search(strtolower($key), $fields_array) !== FALSE) ? $user[$value] : "";
                }
            }
            array_push($ret, $row);
        }
        return $ret;
    }

    function cfpr_ldap_single_search($user_dn, $password, $filter, $field, $dn) {
        try{
              $result = cfpr_ldap_get_single_attribute_list(
                        $this->ldap_url,
                        $user_dn,
                        $dn,
                        $filter,
                        $field,
                        "subtree",
                        "sasl",
                        $password, 1, 100,$this->use_starttls);
            
       }catch(Exception $e){
            $this->set_error('ldap_value_grabbing_error',$e->getMessage());
            log_message('error', 'Error grapping value from directory service single search, '.$e->getMessage().'  '. $e->getLine());
             //$this->set_error($e->getMessage());
             return array();
       }
        $ret = json_decode($result);
        if (is_array($ret)) {
            return $ret;
        }
        return array();
    }

    public function set_warning($warning,$cause=""){
        $this->warnings[]=$warning;
        $this->warning_cause[]=$cause;
        return $warning;
    }
    
    
    public function set_error_delimiters($start_delimiter, $end_delimiter) {
        $this->error_start_delimiter = $start_delimiter;
        $this->error_end_delimiter = $end_delimiter;
        return TRUE;
    }

    public function set_error($error,$cause="") {
        if(!$this->authenticated){
        $this->errors[] = $error;
        $this->cause[]=$cause;
        }
        return $error;
    }

    public function get_unformatted_error(){
        return $this->errors;
    }

    public function errors() {
        $_output = '';
        $i=0;
        foreach ($this->errors as $error) {
            $_output .= $this->error_start_delimiter . $this->cause[$i] .', '.$this->ci->lang->line($error) .$this->error_end_delimiter;
            $i++;
        }
        return $_output;
    }
    
    public function warnings() {
        $_output = '';
        $i=0;
        foreach ($this->warnings as $warning) {
            $_output .= $this->warning_start_delimiter . $this->warning_cause[$i] .', '.$this->ci->lang->line($warning) .$this->warning_end_delimiter;
            $i++;
        }
        return $_output;
    }

    public function set_host($host) {
        $this->hosts = $host;
        $this->ldap_url = "ldap://" . $this->hosts;
    }

    public function get_host() {
        return $this->hosts;
    }

    public function set_basedn($basedn) {
        $this->basedn = $basedn;
    }

    public function get_basedn() {
        return $this->basedn;
    }

    public function set_login_attr($login_attr) {
        $this->login_attribute = $login_attr;
        $this->renamedfields = array(strtolower($this->login_attribute) => 'name', 'mail' => 'email', 'cn' => 'displayname', 'givenname' => 'name');
    }

    public function get_login_attr() {
        return $this->login_attribute;
    }

    public function set_user_dir($user_dir) {
        $this->user_directories = explode(';',$user_dir);
        if(count($this->user_directories)==1){
            $this->users_directory= $this->user_directories[0];
        }
    }

    public function get_user_dir() {
        return implode(';',$this->user_directories);
    }

    /*public function set_member_attr($mem_attr) {
        $this->member_attribute = $mem_attr;
    }

    public function get_member_attr() {
        return $this->member_attribute;
    }*/

    public function set_ad_domain($ad_domain) {
        $this->ad_domain = $ad_domain;
    }

    public function get_ad_domain() {
        return $this->ad_domain;
    }

    public function set_mode($mode) {
        $this->mode = $mode;
        if ($this->mode == 'ldap') {
            $this->use_ad = false;
        } elseif ($this->mode == 'active_directory') {
            $this->use_ad = true;
        } else {
            $this->use_ad = false;
        }
    }

    public function get_mode() {
        return $this->mode;
    }
    
    public function get_encryption(){
        return $this->encryption;
    }
    
    public function set_encryption($mode){
        $this->encryption=$mode;
         if($this->encryption=='ssl'){
            $this->use_ssl=true;
            $this->ldap_url = "ldaps://" . $this->hosts;
        }elseif($this->encryption=='start_tls'){
           $this->use_starttls=true;
        }else{
           $this->encryption='plain' ;
        }
    }
    
    public function set_user_dn($user_dn) {
        $this->user_dn = $user_dn;
    }

    public function get_user_dn() {
        return $this->user_dn;
    }
    

}

?>
