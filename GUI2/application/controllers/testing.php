<?php

class Testing extends CI_Controller {

    function Testing() {
        parent::__construct();
        initializeHub();
        $this->load->library('mongo_db');
    }
    
    function index() {
        
    }

    function mongodbtest() {

        $output = $this->mongo_db->insert('app_logs', array(
            // Server Info
            'server_name' => $_SERVER['SERVER_NAME'],
            'server_ip' => (isset($_SERVER['SERVER_ADDR'])) ? $_SERVER['SERVER_ADDR'] : '0.0.0.0',
            'domain' => (!empty($_SERVER['HTTP_HOST'])) ? $_SERVER['HTTP_HOST'] : '',
            //User Info
            'user_agent' => (!empty($_SERVER['HTTP_USER_AGENT'])) ? $_SERVER['HTTP_USER_AGENT'] : '',
            'ip_address' => (!empty($_SERVER['REMOTE_ADDR'])) ? $_SERVER['REMOTE_ADDR'] : '',
            'uri' => (!empty($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : '',
            'query_string' => (!empty($_SERVER['QUERY_STRING'])) ? $_SERVER['QUERY_STRING'] : '',
            'timestamp' => date('Y-m-d H:i:s'),
            'message' => "testing",
            'level' => "0",
                ));
        echo $output;
    }

    function mongodbselect() {
        $this->load->library('mongo_db');

        //$result=$this->mongo_db->get('users');
        //$result = $this->mongo_db->get('app_logs');
        $result = $this->mongo_db->get('appsettings');
        //$result=$this->mongo_db->get('onlineusers');
        //$this->mongo_db->where(array('group'=>'faculty'));
        //$result=$this->mongo_db->get('work_logs');
        //$result=$this->mongo_db->select(array('username'))->get_where('users',array('group'=>'admin'));
        //print_r($result) ;
        foreach ($result as $docs) {
            //print_r($value)  ;
            $obj = (object) $docs;
            /* foreach ($docs as $key => $value) {
              echo $key.$value;
              echo '</br>';
              } */
            /* echo $docs['uri'];

             */
            // echo $obj->user_name.' ' ;

            print_r($docs);
            /* foreach ($obj->group as $group )
              {
              echo $group.'</br>';
              } */
            echo '</br>';
        }
    }

    function mongodbinput() {
        $this->load->library('ion_auth');
        $this->load->model('ion_auth_model_mongo');
        $result1 = $this->ion_auth_model_mongo->register('admin', 'admin', ' ', array('admin'));
        $result2 = $this->ion_auth->create_group(array('name' => 'admin'));
        echo $result1 . ' ' . $result2;
    }

    function pwdtest() {
        $this->load->model('ion_auth_model_mongo');
        $hased = $this->ion_auth_model_mongo->hash_password_db('test', '2a70c656eb');
        echo $hased;
    }

    function mongodbupdate() {
        $this->load->library('mongo_db');
        $this->mongo_db->where(array('_id' => new MongoId('4d36a0424a235a8024000000')));
        $result = $this->mongo_db->update('users', array('password' => 'password'));
        print_r($result);
    }

    function mysqltest() {
        $this->load->database();
        //$dbhandle = mysql_connect("localhost", "root", "") 
        // or die("Unable to connect to MySQL");
        echo "Connected to MySQL<br>";
    }

    function deleteonlineusers() {
        $this->load->library('mongo_db');
        $this->mongo_db->delete('onlineusers');
    }

    function logintest() {
        /* $this->load->config('ion_auth', TRUE);
          $this->identity_column = $this->config->item('identity', 'ion_auth');
          $this->load->library('mongo_db');
          $result=$this->mongo_db->select(array($this->identity_column, '_id', 'password','group'))
          ->where(array($this->identity_column => 'sudhir','active' => 1))
          ->limit(1)
          ->get_object('users');
          echo $result->password; */

        $this->load->library('ion_auth');
        $result = $this->ion_auth->login('administrator', 'password');
        echo $result;
    }

    function isadmintest() {
        $this->load->library('ion_auth');
        $result = $this->ion_auth->login('administrator', 'password');
        if ($result)
            echo $this->ion_auth->is_admin();
    }

    function get_users() {
        $this->load->library('ion_auth');
        //$user=$this->ion_auth->get_user('4d36a0424a235a8024000000');
        $result = $this->ion_auth->get_users_array();
        print_r($result);

        // print_r ($user->_id);
    }

    function remove_user() {
        $this->load->library('ion_auth');
        $result = $this->ion_auth->delete_user('4d3846c14a235a755f000008');
        print_r($result);
    }

    function test_group() {
        $this->load->library('ion_auth');
        //$result=$this->ion_auth->create_group(array('name'=>'manager'));
        $result = $this->ion_auth->get_groups();
        /* foreach($result as $group)
          {
          echo $group['name'].' '.$group['_id'].'<br/>';
          } */
        print_r($result);
    }

    function test_update() {
        $this->load->library('mongo_db');
        $this->mongo_db->where(array('group' => 'faculties'));
        $this->mongo_db->update('users', array('group.$' => "faculty"));
        $this->mongo_db->clear();
        $result = $this->mongo_db->get('users');
        print_r($result);
    }

    function update_remove() {
        $this->load->library('mongo_db');
        $this->mongo_db->where(array('group' => 'faculties'));
        $result = $this->mongo_db->update_remove_field('users', array('groups' => 1));
        print_r($result);
    }

    function test_update_group() {
        $this->load->model('ion_auth_model_mongo');
        $this->load->library('ion_auth');
        //$result=$this->ion_auth->create_group(array('name'=>'manager'));
        //$result=$this->ion_auth_model_mongo->upate_group('4d3b187a4a235a5366000015',array('name'=>'manager'));
        //echo $result.$this->ion_auth->messages();

        $result = $this->ion_auth_model_mongo->get_group($group->_id != $id);
        if ($result_id != '4d3b187a4a235a5366000015') {
            $this->ion_auth->set_error('group_creation_duplicate');
        }
        echo $this->ion_auth->errors();
    }

    function test_login_remmembered() {
        $this->load->helper('cookie');
        $this->load->library('mongo_db');
        $result = $this->mongo_db->select(array($this->identity_column, '_id', 'group'))
                ->where(array('username' => get_cookie('identity'), 'remember_code' => get_cookie('remember_code')))
                ->limit(1)
                ->get_object('users');
        print_r($result);
    }

    function mail_test() {
        $this->load->library('email');
        $this->email->from('Sudhir.Pandey@cfengine.com', 'cfengine');
        $this->email->to('sudhir2pandey@gmail.com');

        $this->email->subject('Email Test');
        $this->email->message('Testing the email class.');

        $this->email->send();

        echo $this->email->print_debugger();
    }

    function mongoextensiontest() {
        $connection = new Mongo('localhost:27017');
        // Select a database
        $db = $connection->blog;
        //select tables
        //$posts = $db->posts;
        $document = array(
            'title' => 'test',
        );
        $result = $db->posts->insert($document);
        print_r($result);
    }

    function mongoclasstest() {
        $this->load->library('mongo_db');
    }

    function test_json() {
        $string = cfpr_show_green_hosts("green");
        $jsonarr = json_decode($string, true);
        $host = array();

        foreach ($jsonarr as $data) {
            //array_push(&$keys,$data['key'])
            //array_push(&$names,$data['id'])
            $host[$data['key']] = $data['id'];
        }
        /* foreach ($jsonarr as $obj)
          {

          echo $obj['key']."<br/>";
          echo $obj['id']."<br/>";

          }
         */
        print_r($host);
    }

    function allhostjson() {
        $result = cfpr_select_reports(".*", 100);
        print_r(json_decode($result));
    }

    function virtualdesktop() {
        $this->load->view('testviews/MUI_test/virtual_desktop');
    }

    function loadreports() {
        $this->load->helper('html');
        $this->load->view('testviews/MUI_test/reportslist');
    }

    function jsondata() {
        $reports = json_decode(cfpr_select_reports(".*", 100));
        $adjacencies = array();
        $rootnode = array("id" => "node0", "name" => "", "data" => array("\$type" => "none"));
        $control = array();

        $i = 1;
        foreach ($reports as $report) {
            $node = array(
                'nodeTo' => 'node' . $i,
                'data' => array("\$type" => 'none')
            );
            $adjacencies[$i - 1] = $node;
            $i++;
        }
        $rootnode['adjacencies'] = $adjacencies;
        array_push($control, $rootnode);

        $i = 1;
        foreach ($reports as $report) {
            $node_property = array(
                'id' => 'node' . $i,
                'name' => $report,
                'data' => array("\$angularwidth" => "20", "\$color" => "#bf0", "\$height" => 70 + $i),
                'adjacencies' => array()
            );
            array_push($control, $node_property);
            $i++;
            echo $this->_rand_colorCode() . '<br />';
        }

        //print_r($nodelist);
        //print_r(json_encode($control));
    }

    function software_installed_autocomplete($column) {
        $data = cfpr_report_software_in(NULL, NULL, NULL, NULL, true, NULL, NULL, NULL);
        $decoded_data = json_decode($data);
        $column_index = $decoded_data->meta->header->$column;
        $column = array();
        foreach ($decoded_data->data as $rows) {
            array_push($column, $rows[$column_index]);
        }
        $unique_value = array_unique($column);

        if (is_array($unique_value)) {
            $val = array_values($unique_value);
            echo json_encode($val);
        } else {
            echo json_encode($unique_value);
        }
    }

    function host_availabe() {
        $result = json_decode(cfpr_select_hosts("none", ".*", 100));
        $allhosts = array();
        if (count($result) > 0) {
            foreach ($result as $cols) {
                $allhosts[$cols->id] = $cols->key;
            }
        }
        print_r($allhosts);
    }

    function newui() {
        //$this->view->load('testviews/indexnew');
        $this->lang->load('cf_message');
        $this->load->library(array('breadcrumb', 'breadcrumblist'));
        $bc = array(
            'title' => 'Cfengine mission Portal',
            'url' => 'welcome/index',
            'isRoot' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->data = array(
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->load->view('testviews/indexnew', $this->data);
    }

    function filterhost() {
        $this->load->view('testviews/test');
    }

    function cfclasses() {
        //$this->load->library('session');
        //$arr=json_decode(cfpr_class_cloud($this->session->userdata('lastclasslist')));
        echo cfpr_report_classes(NULL, NULL, true, NULL, NULL, NULL);
        ///echo json_encode($arr->classes);
    }

    function listHost() {

        var_dump($_POST);
    }

    function buildfilter($recentfilter) {
        //function call applying recent filter;
        $this->load->library('session');
        $listhost = "sds";
        $this->session->set_userdata('resulthostlist', $listhost);
        if (is_set($this->session->userdata('resulthostlist'))) {
            //function call
            $classes = "";
        } else {
            //function call
            $classes = "";
        }
        return $classes;
    }

    function classCloud() {
        echo base_url() . '/logs/onlineusers.tmp';
    }

    function goals() {
        $goals = json_decode(cfpr_list_business_goals());
        foreach ($goals as $goal) {
            echo $goal->desc;
        }
    }

    function license() {
        $expirydate = strtotime(cfpr_getlicense_expiry());
        $startDate = cfpr_getlicense_installtime();
        //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
        $datediff = $expirydate - $startDate;
        $totaldays = floor($datediff / (60 * 60 * 24));
        echo $totaldays . "<br />";
        echo time() . "<br/>";
        echo date('D F d h:m:s Y', time());
        $dayspassed = floor((time() - $startDate) / (60 * 60 * 24));
        //echo $dayspassed;
        //$pbarvalue=floor(($dayspassed/$totaldays)*100);
        //echo $pbarvalue;
    }

    function topn() {
        print_r(json_decode(cfpr_top_n_hosts("compliance", 1000, 10, 1), true));
    }

    function bluehost() {
        echo cfpr_show_blue_hosts(20, 1);
    }

    function getonlineuserwork() {
        $this->load->library('userdata');
        $users = getonlineusernames();
        foreach ($users as $user) {
            echo $user . ' : ' . $this->userdata->get_personal_working_log_latest($user) . '<br/>';
        }
    }

    function insert_work() {
        $this->load->library('userdata');
        //$this->userdata->insert_personal_working_log("working on test");
        //var_dump($this->userdata->get_personal_working_log_latest('sudhir'));
        //$workinon=$this->userdata->get_personal_working_log_latest('mark');
        $workinon = $this->userdata->get_personal_working_notes('sudhir');
        var_dump($workinon);
    }

    function svnstat() {
        // echo svn_client_version();
        $status = svn_status('./policies/sudhir', SVN_ALL);
        //print_r(svn_status('./policies/sudhir',SVN_ALL));

        /* for ($i=1;$i<2;$i++)
          {
          echo  $status[$i]['revision'];
          } */
        $modified_files = array();
        foreach ($status as $file) {
            if ($file['text_status'] == 8) {
                array_push($modified_files, $file['name']);
            }
        }
        print_r($modified_files);
    }

    function viewlogs() {
        $this->load->model('repository_model');
        var_dump($this->repository_model->get_svn_logs('https://c.cfengine.com/svn/sudhir'));
    }

    function viewrev() {
        $this->load->model('repository_model');
        var_dump($this->repository_model->get_revisions('https://c.cfengine.com/svn/sudhir'));
    }

    function currepo() {
        $params = array(
            'workingdir' => get_policiesdir() . $this->session->userdata('username')
        );
        $this->load->library('cfsvn', $params);
        echo $this->cfsvn->get_current_repository();
    }

    function currentversion() {
        $params = array(
            'workingdir' => get_policiesdir() . $this->session->userdata('username')
        );
        $this->load->library('cfsvn', $params);
        echo $this->cfsvn->get_working_revision();
    }

    function getapprovedpolicies() {
        $this->load->library('mongo_db');
        var_dump($this->mongo_db->get('approvedpolicies'));
        // var_dump($this->repository_model->get_all_approved_policies('https://c.cfengine.com/svn/sudhir'));
    }

    function getapprovedpoliciescount() {
        $params = array(
            'workingdir' => get_policiesdir() . $this->session->userdata('username')
        );
        $this->load->library('cfsvn', $params);
        $this->load->model('repository_model');
        $current_repo = $this->cfsvn->get_current_repository();
        echo $this->repository_model->get_total_approval_count($current_repo);
    }

    function svn_logs() {

        $this->load->model('repository_model');
        try {
            $params = array(
                'workingdir' => get_policiesdir() . $this->session->userdata('username')
            );
            $this->load->library('cfsvn', $params);
            $currentUser = $this->session->userdata('username');
            $current_repo = $this->cfsvn->get_current_repository();
            $obj = $this->repository_model->get_specific_repository($currentUser, $current_repo);

            if ($obj != NULL) {
                $info = array('userId' => $obj->userId, 'password' => $obj->password);
                $username = $obj->username;
                $password = $this->repository_model->decrypt_password($info);
                $params = array(
                    'username' => $username,
                    'password' => $password,
                    'repository' => $current_repo
                );
            }
            $this->cfsvn->addcredentials($params);

            //var_dump($this->cfsvn->get_current_repository());
            // echo $this->cfsvn->get_current_repository();
            var_dump($this->cfsvn->cfsvn_log(10));
        } catch (Exception $e) {
            var_dump($e);
        }
    }

    function vitals_week_test() {
        /* $ret_host = cfpr_vitals_list("SHA=38c5642ccb0dc74bc754aa1a63e81760869e1b3405bf9e43ad85c99822628e8e");
          echo $ret_host."/n";
          $ret_old=cfpr_performance_analysis("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e");
          echo $ret_old;
          $ret=cfpr_vitals_list("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e");
          //$ret=cfpr_vitals_view_week("SHA=38c5642ccb0dc74bc754aa1a63e81760869e1b3405bf9e43ad85c99822628e8e",1);
          echo $ret; */
        echo "<h1>vitals</h1><br /><br />";
        echo cfpr_performance_analysis("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e") . "<br /><br />";
        echo cfpr_vitals_list("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e") . '<br />';
        echo "<h1>weekley</h1><br /><br />";
        echo cfpr_get_weekly_view("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e", "26") . "<br /><br />";
        echo cfpr_vitals_view_week("SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e", "wwws_out") . "<br/>";
    }

    function vitals_list_test() {
        $list = cfpr_vitals_list(NULL);
        echo $list;
        $newlist = json_decode($list, true);
        var_dump($newlist);
    }

    function testcurl() {
        $_data = array("name" => "sudhir", "login" => true);
        $data = array();
        while (list($n, $v) = each($_data)) {
            $data[] = urlencode($n) . "=" . urlencode($v);
        }
        $data = implode('&', $data);
        $ch = curl_init();

        // set URL and other appropriate options
        curl_setopt($ch, CURLOPT_URL, "http://localhost:8080/start");
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        // grab URL and pass it to the browser
        curl_exec($ch);

        // close cURL resource, and free up system resources
        curl_close($ch);
    }

    function notifier_test() {
        $this->load->library('onlineUsers');
        $_data = array('event_loggedin' => true, 'ttlusr' => $this->onlineusers->total_users());
        $url = get_nodehost_from_server() . '/userloggedin';
        var_dump($url);
        notifier($url, $_data);
    }

    function get_keys() {
        $this->load->model('repository_model');
        $currentUser = $this->session->userdata('username');
        echo "getting repositoriy info <br/>";
        $obj = $this->repository_model->get_specific_repository($currentUser, "https://c.cfengine.com/svn/sudhir");
        var_dump($obj);
        echo "getting keys <br/>";
        $keys = $this->repository_model->get_key(array('userId' => $obj->userId));
        var_dump($keys);
        echo "encrytp password<br ";
        $password = $this->repository_model->encrypt_password(array('password' => 'wachnapmadu', 'userId' => $obj->userId));
        echo $password . "<br />";
        echo "decrypting passwordkeys<br/>";
        $password = $this->repository_model->decrypt_password(array('userId' => $obj->userId, 'password' => $obj->password));
        echo $password;
    }

    function version() {
        echo cfpr_enterprise_version();
    }

    function timezoneset($offset) {
        $offset = $offset * 60 * 60;
        $abbrarray = timezone_abbreviations_list();
        foreach ($abbrarray as $abbr) {
            //echo $abbr."<br>";
            foreach ($abbr as $city) {
                //echo $city['offset']." $offset<br>";
                if ($city['offset'] == $offset) { // remember to multiply $offset by -1 if you're getting it from js
                    date_default_timezone_set($city['timezone_id']);
                    $data = date_default_timezone_get();
                    var_dump($data);
                    return true;
                }
            }
        }
    }

    function testldap() {
        $this->load->library('Auth_Ldap');
        $result = $this->auth_ldap->login('sudhir', 'Cf3ng1n3');
        if ($result) {
            print_r($result);
            echo "logged in";
        } else {
            echo "not ";
        }
    }

    function load_ldap_users() {
        $this->load->library('Auth_Ldap');
        //$values=$this->auth_ldap-> getallLdapUsers();
        $values = $this->auth_ldap->get_all_ldap_users('sudhir', 'Cf3ng1n3');
        print_r($values);
    }

    function load_ldap_groups() {
        $this->load->library('Auth_Ldap');
        //$values=$this->auth_ldap-> getallLdapUsers();
        $values = $this->auth_ldap->get_all_ldap_groups('sudhir', 'q1w2e3r4t5');
        print_r($values);
    }

    function setting_get_item() {
        $this->load->model('settings_model');
        var_dump($this->settings_model->get_app_settings());
    }

    function test_cfpr_ldap() {
        if (function_exists("cfpr_ldap_authenticate")) {
            //$result=cfpr_ldap_authenticate('ldaps://cf022osx.cfengine.com:636','uid=sudhir,cn=users,dc=cf022osx,dc=cfengine,dc=com','q1w2e3r4t5');
            $result=cfpr_ldap_authenticate('ldap://10.0.0.152','uid=njoshi,ou=sales,dc=cfengine,dc=com','password');
            //$result = cfpr_ldap_authenticate('ldap://10.0.0.35', 'sudhir@windows1.test.cfengine.com', 'Cf3ng1n3');
            var_dump($result);
        }
    }

    function test_cfpr_ldap_attr() {
        //$result=cfpr_ldap_get_several_attributes("ldap://10.0.0.100","uid=sudhir,cn=users,dc=cf022osx,dc=cfengine,dc=com","cn=users,dc=cf022osx,dc=cfengine,dc=com","(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))","uid,mail,sn,altSecurityIdentities","subtree","sasl","q1w2e3r4t5","2",100);
        $result = cfpr_ldap_get_several_attributes("ldap://10.0.0.152", "uid=sudhir", "ou=people,dc=cfengine,dc=com", "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))", "uid,cn,mail", "subtree", "sasl", "password", 1, 100);
        $users = json_decode($result, true);
        $ret = array();
        foreach ($users['data'] as $user) {
            $row = array();
            foreach ($users['keys'] as $key => $value) {
                $row[$key] = $user[$value];
            }
            array_push($ret, $row);
        }
        var_dump($ret);
    }

    function test_cfpr_ldap_single_attr() {
        $result = cfpr_ldap_get_single_attribute_list("ldap://10.0.0.152", "uid=sudhir,ou=people,dc=cfengine,dc=com", "ou=people,dc=cfengine,dc=com", "(uid=sudhir)", "mail", "subtree", "sasl", "password", 1, 100);
        var_dump($result);
    }
    
    function test_cfpr_ldap_single_attr_ssl() {
        $result = cfpr_ldap_get_single_attribute_list("ldaps://10.0.0.100:636", "uid=sudhir,cn=users,dc=cf022osx,dc=cfengine,dc=com", "cn=users,dc=cf022osx,dc=cfengine,dc=com", "(uid=sudhir)", "cn", "subtree", "sasl", "q1w2e3r4t5", 1, 100);
        var_dump($result);
    }

    function test_get_groups() {
        $this->load->library('Auth_Ldap');
        //$values=$this->auth_ldap-> getallLdapUsers();
        $values = $this->auth_ldap->get_role_for_user('sudhir', 'Cf3ng1n3');
        print_r($values);
        //var_dump($result);
    }

    function test_get_details_ldap_users() {
        $result = cfpr_ldap_get_several_attributes("ldap://10.0.0.35", 
                    "sudhir@windows1.test.cfengine.com", 
                    "dc=windows1,dc=test,dc=cfengine,dc=com", 
                    "(samaccountname=sudhir)", 
                   "sAMAccountName,cn,distinguishedName",
                   "subtree", "sasl", "Cf3ng1n3", 
                 1, 100);
        /* $this->load->library('Auth_Ldap');
          $result=$this->auth_ldap->get_details_for_user('sudhir','Cf3ng1n3'); */
        var_dump($result);
    }

    function test_get_all_adgroups() {
        try{
        $result = cfpr_ldap_get_several_attributes("ldaps://10.0.0.35:636", 
                "sudhir@windows1.test.cfengine.com", 
               // "CN=jon henrik,CN=Users,DC=windows1,DC=test,DC=cfengine,DC=com",
                "dc=windows1,dc=test,dc=cfengine,dc=com", 
                "(&(objectClass=user))", 
                "sAMAccountName", "subtree", "sasl", "Cf3ng1n3", 1, 100);
        var_dump($result);
        }
        catch(Exception $e){
            echo $e->getMessage();
        }
    }

    function test_get_all_ldapusers(){
        try{
            $result=cfpr_ldap_get_several_attributes("ldap://10.0.0.100",
                     "uid=sudhir,cn=users,dc=cf022osx,dc=cfengine,dc=com",
                     "dc=cf022osx,dc=cfengine,dc=com",
                     "(|(objectClass=organizationalPerson)(objectClass=inetOrgPerson))",
                     "uid,cn","subtree", "sasl", "q1w2e3r4t5", 1, 100);
            var_dump($result);
             }catch(Exception $e){
                  echo $e->getMessage();
        }
    }
    
    function test_get_users_by_group() {
        $this->load->model('ion_auth_model_mongo');
        $result = $this->ion_auth_model_mongo->get_users_by_group('manager');
        var_dump($result);
    }

    function newlogin() {
        $this->load->helper('form');
        $data['lbl_identifier'] = 'Username';
        $data['identifier'] = array('name' => 'name',
            'id' => 'name',
            'type' => 'text',
            'value' => ''
        );
        $data['password'] = array('name' => 'password',
            'id' => 'password',
            'type' => 'password',
        );
        $this->load->view('auth/loginnew', $data);
    }
    


}