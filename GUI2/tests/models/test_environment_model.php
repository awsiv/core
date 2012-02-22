<?php
class test_environment_model extends CodeIgniterUnitTestCase {
    private $username="admin";
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('environment_model');
    }

    public function tearDown() {
        
    }
    
     public function test_getEnvironmentList(){
         $name = $this->_ci->environment_model->getEnvironmentList($this->username);
         $this->assertTrue(is_array($name),"Array must be returned");
         $this->dump($name); 
    }
}
?>
