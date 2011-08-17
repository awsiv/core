<?php
class test_ldap_libraries extends CodeIgniterUnitTestCase {
private $username='sudhir';
private $password='Cf3ng1n3';//'q1w2e3r4t5'//password//Cf3ng1n3;
private $dn='CN=Sudhir Pandey,CN=Users,DC=windows1,DC=test,DC=cfengine,DC=com';
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
       $this->_ci->load->library('Auth_Ldap');
    }

    public function tearDown() {

    }

    public function test_php_ldap_library(){
         $this->assertTrue(function_exists("ldap_connect"),"Should be true if there is Php-ldap module loaded");
    }

   
     public function test_ldap_settings(){
        $settings=array('host'=>$this->_ci->auth_ldap->get_host(),
                              'base_dn'=>$this->_ci->auth_ldap->get_basedn(),
                              'login_attr'=>$this->_ci->auth_ldap->get_login_attr(),
                               'user_dir'=>$this->_ci->auth_ldap->get_user_dir(),
                                'member_attr'=>$this->_ci->auth_ldap->get_member_attr(),
                               'ad_domian'=>$this->_ci->auth_ldap->get_ad_domain() ,
                               'mode'=>$this->_ci->auth_ldap->get_mode(),
                        );
        $this->dump($settings);
    }
    
    public function test_php_ldap_login(){
       $ret=$this->_ci->auth_ldap->login($this->username,$this->password);
       $this->assertTrue(is_array($ret),"Must succesfully login into system");
      $this->dump($ret);
    }

    public function test_php_ldap_inbvalidLogin(){
        $this->assertFalse($this->_ci->auth_ldap->login('asdas','sadasdas'),"Must be a invalid login attempt");
    }

    public function test_php_ldap_getallusers(){
         $ret=$this->_ci->auth_ldap-> get_all_ldap_users($this->username,$this->password);
         $this->assertTrue(is_array($ret),"Should be  an array of list of users");
         if(!empty($ret)){
             $this->assertTrue(key_exists('name', $ret[0]),"should have name as one of the attribute");
             $this->assertTrue(key_exists('displayname', $ret[0]),"should have displayname as one of the attribute");
              $this->assertTrue(key_exists('email', $ret[0]),"should have email as one of the attribute");
         }
         $this->dump($ret);
    }

    public function test_php_ldap_getallgroups_for_user(){
       // $ret=$this->_ci->auth_ldap->get_role_for_user($this->username,$this->password);
        //for openldap the third parameter is use less
        $ret=$this->_ci->auth_ldap->get_role_for_user($this->username,$this->password,$this->dn);
         $this->assertTrue(is_array($ret),"Should be  an array of list of groups");
         $this->dump($ret);
    }
   public function test_php_ldap_getallgroups(){
        $ret=$this->_ci->auth_ldap->get_all_ldap_groups($this->username,$this->password);
        $this->assertTrue(is_array($ret),"Should be  an array of list of users");
        $this->dump($ret);
    }
}
?>
