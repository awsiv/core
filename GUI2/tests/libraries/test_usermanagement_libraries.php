<?php

class test_usermanagement_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->library('ion_auth');
    }

    public function tearDown() {
        
    }

    public function test_ldap_user_role_exits() {
        $data = $this->_ci->ion_auth->ldap_user_role_entry_exits("sudhir");
        $this->assertFalse($data);
    }

    public function test_update_ldap_user() {
        $data = $this->_ci->ion_auth->update_ldap_users("test", array("admin", "developer"));
        $this->assertTrue($data);
    }

    public function test_update_local_user() {
        $id = "4f0da4014a235a7725000001";
        $data = array(
            'username' => "sudhir",
            'email' => "sudhir.pandey@cfengine.com",
            'roles' => array("admin", "developer", "testrole"),
        );
        $this->_ci->ion_auth->update_user($id, $data);
        $roles = $this->_ci->ion_auth->get_user_rolelist($id);
        //$this->dump($roles);
        $count = count($roles);
        $this->assertEqual(3, $count);
        $allelements = false;
        foreach ($data['roles'] as $role) {
            in_array($role, $roles) ? $allelements = true : $allelements = false;
        }
        $this->assertTrue($allelements);
    }

    public function test_is_any_ldap_user_entry_present() {
        $ret = $this->_ci->ion_auth->is_ldap_user_exists();
        $this->assertTrue($ret);
    }

    public function test_get_ldap_user_from_local_db() {
        $ret = $this->_ci->ion_auth->get_ldap_user_details_from_local_db("test");
        $this->assertNotNull($ret, "The user test must be there");
        $this->dump($ret);
    }

    //-------------  Roles --------------------------------
    /*
     * will run cfpr_role_create(username, name,desc,crxi,crxx,brxi) in the end
     */
    public function test_cfpr_role_create() {
        $username = 'admin';        
        $data = array(
            'name' => "test_role_" . date("d_M_Y_H_i_s") . time(),
            'description' => "Test role. Created at: " . date("d/M/Y H:i:s"),
            'crxi' => "crxi",
            'crxx' => 'crxx',
            'brxi' => 'brxi',
            'brxx' => 'brxx'
        );
        $ret = $this->_ci->ion_auth->create_role($username, $data);

        $this->assertTrue($ret, 'New role created. cfpr_role_create() returned 1');
    }
    
    /*
     *  will run cfpr_role_update(username, name) in the end
     * 
     * We are not allow to change role name
     */
    public function test_cfpr_role_update() {
        $username = 'admin';
        $data = array(
            'name' => "test_role_" . date("d_M_Y_H_i_s") . '_' . rand(2346, 6789) . '_' . time(),
            'description'     => "Test role. Created at: " . date("d/M/Y H:i:s"),
            'crxi' => "crxi",
            'crxx' => 'crxx',
            'brxi' => 'brxi',
            'brxx' => 'brxx'
        );

        $ret = $this->_ci->ion_auth->create_role($username, $data);
        
        if ($ret === true) {
                $data['description'] = "Test role. UPDATED AT: " . date("d/M/Y H:i:s");
                $data['crxi'] = "crxi UPDATED";
                $data['crxx'] = 'crxx UPDATED';
                $data['brxi'] = 'brxi UPDATED';
                $data['brxx'] = 'brxx UPDATED';
                $ret = $this->_ci->ion_auth->update_role($username, $data);
        }

        $this->assertTrue($ret, 'Role  ' . $data['name'] . ' updated. cfpr_role_update() returned 1. Note this test depend on result from test_cfpr_role_create()');
    }    

    /*
     *  will run cfpr_role_delete(username, name) in the end
     */

    public function test_cfpr_role_delete() {
        $username    = 'admin';
        $ret         = false;
        $create_role = false;
        
        $data = array(
            'name' => "test_role_" . date("d_M_Y_H_i_s") . '_' . rand(0, 2345) . '_' . time(),
            'description'     => "Test role. Created at: " . date("d/M/Y H:i:s"),
            'crxi' => "crxi",
            'crxx' => 'crxx',
            'brxi' => 'brxi',
            'brxx' => 'brxx'
        );

        $create_role = $this->_ci->ion_auth->create_role($username, $data);
        $this->assertTrue($create_role, 'Role  ' . $data['name'] . ' created for delete');
        
        
        if ($create_role === true) {
            $ret = $this->_ci->ion_auth->delete_role($username, $data['name']);
        }

        $this->assertTrue($ret, 'Role  ' . $data['name'] . ' deleted. cfpr_role_delete() returned 1. Note this test depend on result from test_cfpr_role_create()');
    }

    /*
     *  will run cfpr_role_list_all(username) in the end
     */    
    public function test_cfpr_role_list_all() {
        $username = 'admin';

        $ret = $this->_ci->ion_auth->get_roles($username);

        $this->assertNotNull($ret, 'Role list vissble by admin user ');
        $this->dump($ret);
        
        //TODO: add test for another condition - as empty username etc
    }

}

?>