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
    
     public function test_ldap_user_role_exits(){
       $data=$this->_ci->ion_auth->ldap_user_role_entry_exits("sudhir");
       $this->assertFalse($data);
    }
    
    public function test_update_ldap_user(){
        $data=$this->_ci->ion_auth->update_ldap_users("test",array("admin","developer"));
        $this->assertTrue($data);
    }
    
    public function test_update_local_user(){
        $id="4f0da4014a235a7725000001";
        $data = array(
                'username' => "sudhir",
                'email' => "sudhir.pandey@cfengine.com",
                'roles' => array("admin","developer","testrole"),
            );
        $this->_ci->ion_auth->update_user($id,$data);
        $roles=$this->_ci->ion_auth->get_user_rolelist($id);
        //$this->dump($roles);
        $count=count($roles);
        $this->assertEqual(3, $count);
        $allelements=false;
        foreach($data['roles'] as $role){
            in_array($role, $roles)?$allelements=true:$allelements=false;
        }
        $this->assertTrue($allelements);
    }
    
    public function test_is_any_ldap_user_entry_present(){
        $ret=$this->_ci->ion_auth->is_ldap_user_exists();
        $this->assertTrue($ret);
    }

}
?>
