<?php
class test_class_model extends CodeIgniterUnitTestCase {
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('class_model');
        $this->username = 'admin';

        $this->searchletter = "a.*";
        $this->default_include  = array("cf.*");
        $this->default_exclude  = array("zz.*");
    }

    public function tearDown() {

    }

     public function test_getClassList(){
         $name = $this->_ci->class_model->getAllClasses($this->username, null, array( ".*"), array(''), 10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name);
    }

    public function test_getClassListWithParam(){
         $name = $this->_ci->class_model->getAllClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude, 10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name);
    }

    public function test_getAllTimeClasses(){
         $name = $this->_ci->class_model->getAllTimeClasses($this->username, null, ".*", '',10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name);
    }


    public function test_getAllTimeClassesWithParam(){
         $name = $this->_ci->class_model->getAllTimeClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude, 10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name);
    }

    public function test_getAllSoftClasses(){
         $name = $this->_ci->class_model->getAllSoftClasses($this->username, null,".*", '',10,1);
         $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
         $this->dump($name);
    }


    public function test_getAllSoftClassesWithParam(){
        $name = $this->_ci->class_model->getAllSoftClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude,10,1);
        $this->assertTrue(is_array(json_decode($name)),"Must be a valid set of class");
        $this->dump($name);
    }
}
?>