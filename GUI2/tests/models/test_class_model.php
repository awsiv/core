<?php
class test_class_model extends CodeIgniterUnitTestCase {
    private $username="admin";
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('class_model');
    }

    public function tearDown() {
        
    }
    
     public function test_getClassList(){
         $name = $this->_ci->class_model->getAllClasses($this->username,".*",10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name); 
    }
    
    public function test_getAllTimeClasses(){
         $name = $this->_ci->class_model->getAllTimeClasses($this->username,".*",10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name); 
    }
    
      public function test_getAllSoftClasses(){
         $name = $this->_ci->class_model->getAllSoftClasses($this->username,".*",10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name); 
    }
}
?>
