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

    function __construct() {
        $this->ci = & get_instance();
        log_message('debug', 'Auth_Ldap initialization commencing...');

        // Load the session library
        $this->ci->load->library('session', 'ion_auth');
        $this->ci->load->config('ion_auth', TRUE);
        $this->ci->load->model('settings_model');
        // Load the configuration
        //$this->ci->load->config('auth_ldap');
        $this->error_start_delimiter = $this->ci->config->item('error_start_delimiter', 'ion_auth');
        $this->error_end_delimiter = $this->ci->config->item('error_end_delimiter', 'ion_auth');

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
            $this->login_attribute = 'samaccountname';
        } elseif (!$this->use_ad && $this->login_attribute == '') {
            $this->login_attribute = 'uid';
        }
        
        $this->users_directory = $appsettings->users_directory;
        $this->renamedfields = array(strtolower($this->login_attribute) => 'name', 'mail' => 'email', 'cn' => 'displayname', 'givenname' => 'name');
        $this->ldap_url = "ldap://" . $this->hosts;

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
                'group' => $user_info['role'],
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

        /* foreach ($this->hosts as $host) {
          $this->ldapconn = ldap_connect($host);
          if ($this->ldapconn) {
          break;
          } else {
          log_message('info', 'Error connecting to ' . $uri);
          }
          }
          // At this point, $this->ldapconn should be set.  If not... DOOM!
          if (!$this->ldapconn) {
          log_message('error', "Couldn't connect to any LDAP servers.  Bailing...");
          //show_error('Error connecting to your LDAP server(s).  Please check the connection and try again.');
          $this->set_error('Error connecting to your LDAP server(s).  Please check the connection and try again.');
          return false;
          } */

        // We've connected, now we can attempt the login.
        if ($this->use_ad) {
            $entries = "";
            $details = array();
            //set up adconnection giving username and password;
            if ($this->_set_up_ADconnection($username, $password)) {
                $details = $this->get_details_for_user($username, $password);
                if (empty($details)) {
                    $this->set_error('no_entries_found');
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
            $binddn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $bind = cfpr_ldap_authenticate($this->ldap_url, $binddn, $password);
            if (!$bind) {
                $this->_audit("Failed login attempt: " . $username . " from " . $_SERVER['REMOTE_ADDR']);
                 $this->set_error('unsucessful_ldap_bind');
                return FALSE;
            } else {
                $details = $this->get_details_for_user($username, $password);
                if (count($details) == 0) {
                    $this->set_error('no_entries_found');
                    return False;
                }
            }
            $cn = $details[0]['displayname'];
            $dn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $id = $details[0]['name'];
        }

        $roles = array();
        if ($this->use_ad) {
            $roles = $this->get_role_for_user($username, $password, $dn);
        } else {
            $roles = $this->get_role_for_user($id, $password);
        }

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

    /**
     * @access private
     * @param string $username
     * @return array if login is valid
     */
    /* private function _get_role($username) {
      if ($this->use_ad) {
      $escaped = $this->ldap_escape($username, false);
      //show_error(stripslashes($username));
      //show_error($escaped);
      $filter = '(member=' . $escaped . ')';
      $search = ldap_search($this->ldapconn, $this->basedn, $filter, array('cn'));
      if (!$search) {
      log_message('error', "Error searching for group:" . ldap_error($this->ldapconn));
      show_error('Couldn\'t find groups: ' . ldap_error($this->ldapconn));
      }
      $results = ldap_get_entries($this->ldapconn, $search);
      //show_error(nl2br(var_export($results,true)));
      //print_r($results);
      $role = FALSE;
      $roles = array();
      if ($results['count'] != 0) {
      //show_error(nl2br(var_export($results,true)));
      $rolefound = false;
      for ($i = 0; $i < $results['count']; $i++) {
      $role = array_search($results[$i]['cn'][0], $this->roles);
      $roles[$i] = $results[$i]['cn'][0];
      if ($role !== FALSE) {
      //return $role;
      ($rolefound)? : $rolefound = true;
      }
      }
      if ($rolefound) {
      return $roles;
      }
      }
      } else {
      $filter = '(' . $this->member_attribute . '=' . $username . ')';
      $search = ldap_search($this->ldapconn, $this->basedn, $filter, array('cn'));
      if (!$search) {
      log_message('error', "Error searching for group:" . ldap_error($this->ldapconn));
      show_error('Couldn\'t find groups: ' . ldap_error($this->ldapconn));
      }
      $results = ldap_get_entries($this->ldapconn, $search);

      //            show_error(nl2br(var_export($results,true)));
      // print_r($results);

      $roles = array();
      if ($results['count'] != 0) {
      $rolefound = false;
      for ($i = 0; $i < $results['count']; $i++) {
      $role = array_search($results[$i]['cn'][0], $this->roles);
      $roles[$i] = $results[$i]['cn'][0];
      if ($role !== FALSE) {
      // return $role;
      ($rolefound)? : $rolefound = true;
      }
      }
      if ($rolefound) {
      return $roles;
      }
      }
      }
      return false;
      } */

    private function _set_up_ADconnection($username, $password) {
        if (preg_match('/^(\w+\.)+\w{2,4}$/', $this->ad_domain)) {
            $binddn = $username . '@' . $this->ad_domain;
        } else {
            $binddn = $this->ad_domain . '\\' . $username;
        }
        $bind = cfpr_ldap_authenticate($this->ldap_url, $binddn, $password);
        //$bind = @ldap_bind($this->ldapconn, $binddn, $password);
        if (!$bind) {
            log_message('error', 'Unable to perform Active directory bind');
            $this->set_error("unsucessful_active_directory_bind");
            return FALSE;
        }
        return TRUE;
    }

    /* private function _set_up_Openldap() {
      // Find the DN of the user we are binding as
      // If proxy_user and proxy_pass are set, use those, else bind anonymously
      if ($this->proxy_user) {
      $bind = ldap_bind($this->ldapconn, $this->proxy_user, $this->proxy_pass);
      } else {

      //some version of ldap starts ldap in version 3 and php expects it to be in version 3
      ldap_set_option($this->ldapconn, LDAP_OPT_PROTOCOL_VERSION, 3);
      $bind = @ldap_bind($this->ldapconn);
      }

      if (!$bind) {
      log_message('error', 'Unable to perform anonymous/proxy bind');
      show_error('Unable to bind for user id lookup');
      return false;
      }

      log_message('debug', 'Successfully bound to directory Anomously.  Performing dn lookup for ');
      return true;
      } */

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
            $dn = 'cn=users,' . $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,4}$/', $this->ad_domain)) {
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
            $dn = $this->users_directory . ',' . $this->basedn;
            $userdn = $this->login_attribute . '=' . $username . ',' . $dn;
            $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
            return $result;
        }
    }

    /**
     *
     * @param <type> $username
     * @param <type> $password
     * @return <type> Returns array of associative array each conaining different attributes mentioned in the $fields array;
     */
    function get_all_ldap_groups($username, $password) {
        if ($this->use_ad) {
            $filter = '(&(objectCategory=group))';
            $fields = "samaccountname";
            $dn = "ou=groups," . $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,4}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_search($binddn, $password, $filter, $fields, $dn);
            return $result;
        } else {
            $filter = '(' . $this->member_attribute . '=*)';
            $fields = "cn";
            $dn = $this->basedn;
            $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
            return $result;
        }
    }

    function get_role_for_user($username, $password, $get_role_arg=null) {
        if ($this->use_ad) {
            $escaped = $this->ldap_escape($get_role_arg, false);
            $filter = '(member=' . $escaped . ')';
            $field = "cn";
            $dn = "ou=groups," . $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,4}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_single_search($binddn, $password, $filter, $field, $dn);
            return $result;
        } else {
            $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $filter = '(' . $this->member_attribute . '=' . $username . ')';
            $field = "cn";
            $dn = $this->basedn;
            $result = $this->cfpr_ldap_single_search($userdn, $password, $filter, $field, $dn);
            return $result;
        }
    }

    function get_details_for_user($username, $password) {
        $filter = '(' . $this->login_attribute . '=' . $username . ')';
        if ($this->use_ad) {
            $fields = "givenName,cn,distinguishedName";
            $dn = 'cn=users,' . $this->basedn;
            if (preg_match('/^(\w+\.)+\w{2,4}$/', $this->ad_domain)) {
                $binddn = $username . '@' . $this->ad_domain;
            } else {
                $binddn = $this->ad_domain . '\\' . $username;
            }
            $result = $this->cfpr_ldap_search($binddn, $password, $filter, $fields, $dn);      
        } else {
            $fields = "cn,dn,uid";
            $userdn = $this->login_attribute . '=' . $username . ',' . $this->users_directory . ',' . $this->basedn;
            $dn = $this->users_directory . ',' . $this->basedn;
            $result = $this->cfpr_ldap_search($userdn, $password, $filter, $fields, $dn);
        }
        return $result;
    }

    /* function search_ldap($username, $password, $filter, $fields, $dn=Null) {
      foreach ($this->hosts as $host) {
      $this->ldapconn = ldap_connect($host);
      if ($this->ldapconn) {
      break;
      } else {
      log_message('info', 'Error connecting to ' . $uri);
      }
      }
      if (is_null($dn)) {
      $dn = $this->basedn;
      }
      $entries = '';
      if ($this->use_ad) {

      if ($this->_set_up_ADconnection($username, $password)) {
      $ldapsearch = ldap_search($this->ldapconn, $dn, $filter, $fields);
      $entries = ldap_get_entries($this->ldapconn, $ldapsearch);
      }
      if (!is_array($entries) || $entries['count'] == 0) {
      //show_error('General ldap_search error: ' . ldap_err2str(ldap_errno($this->ldapconn)));
      $this->set_error('General ldap_search error: ' . ldap_err2str(ldap_errno($this->ldapconn)));
      return FALSE;
      }
      //print_r($entries);
      } else {
      $anombind = $this->_set_up_Openldap();
      if (!$anombind) {
      $binddn = $this->login_attribute . '=' . $username . 'ou=people' . $this->basedn;
      $bind = @ldap_bind($this->ldapconn, $binddn, $password);
      if (!$bind) {
      $this->_audit("Failed login attempt: " . $username . " from " . $_SERVER['REMOTE_ADDR']);
      return FALSE;
      }
      }
      $ldapsearch = ldap_search($this->ldapconn, $dn, $filter, $fields);
      $entries = ldap_get_entries($this->ldapconn, $ldapsearch);
      if ($entries['count'] == 0) {
      //show_error('General ldap_search error: ' . ldap_err2str(ldap_errno($this->ldapconn)));
      $this->set_error('General ldap_search error: ' . ldap_err2str(ldap_errno($this->ldapconn)));
      return FALSE;
      }
      }
      $results = array();
      for ($i = 0; $i < $entries['count']; $i++) {
      $row = array();
      foreach ($fields as $attrib) {
      if (key_exists($attrib, $this->renamedfields)) {
      $row[$this->renamedfields[$attrib]] = (key_exists($attrib, $entries[$i])) ? $entries[$i][$attrib][0] : "";
      } else {
      $row[$attrib] = (key_exists($attrib, $entries[$i])) ? $entries[$i][$attrib][0] : "";
      }
      }
      array_push($results, $row);
      }
      return $results;
      } */

    function cfpr_ldap_search($user_dn, $password, $filter, $fields, $dn) {
        $result = cfpr_ldap_get_several_attributes($this->ldap_url,
                        $user_dn,
                        $dn,
                        $filter,
                        $fields,
                        "subtree",
                        "sasl",
                        $password, 1, 100);
        $temp_array = explode(',', $fields);
        foreach($temp_array as $val){
            $fields_array[]=strtolower($val);
        }
        $ret = array();
        $users = json_decode($result, true);
        if (!is_array($users)) {
            $this->set_error("Internal_Ldap_module_error");
            return;
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
        $result = cfpr_ldap_get_single_attribute_list(
                        $this->ldap_url,
                        $user_dn,
                        $dn,
                        $filter,
                        $field,
                        "subtree",
                        "sasl",
                        $password, 1, 100);
        $ret = json_decode($result);
        if (is_array($ret)) {
            return $ret;
        }
        return array();
    }

    public function set_error_delimiters($start_delimiter, $end_delimiter) {
        $this->error_start_delimiter = $start_delimiter;
        $this->error_end_delimiter = $end_delimiter;
        return TRUE;
    }

    public function set_error($error) {
        $this->errors[] = $error;
        return $error;
    }

    public function get_unformatted_error(){
        return $this->errors;
    }

    public function errors() {
        $_output = '';
        foreach ($this->errors as $error) {
            $_output .= $this->error_start_delimiter . $this->ci->lang->line($error) . $this->error_end_delimiter;
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
        $this->users_directory = $user_dir;
    }

    public function get_user_dir() {
        return $this->users_directory;
    }

    public function set_member_attr($mem_attr) {
        $this->member_attribute = $mem_attr;
    }

    public function get_member_attr() {
        return $this->member_attribute;
    }

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

}

?>
