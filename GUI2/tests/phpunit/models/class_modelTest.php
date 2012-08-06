<?php
class test_class_model extends CI_BaseTestCase
{

    private $username = "admin";


    public function setUp()
    {
        parent::setUp();
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->ci_obj = new StdClass;

        // Fix get_instance()
        $this->ci_instance($this->ci_obj);

        $this->load->model('class_model');
        $this->username = 'admin';

        $this->searchletter = "a.*";
        $this->default_include = array("cf.*");
        $this->default_exclude = array("zz.*");
    }

    public function test_getClassList()
    {
        $name = $this->ci_instance->class_model->getAllClasses($this->username, null, array(".*"), array(''), 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

    public function test_getClassListWithParam()
    {
        $name = $this->ci_instance->class_model->getAllClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude, 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

    public function test_getAllTimeClasses()
    {
        $name = $this->ci_instance->class_model->getAllTimeClasses($this->username, null, ".*", '', 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

    public function test_getAllTimeClassesWithParam()
    {
        $name = $this->ci_instance->class_model->getAllTimeClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude, 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

    public function test_getAllSoftClasses()
    {
        $name = $this->ci_instance->class_model->getAllSoftClasses($this->username, null, ".*", '', 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

    public function test_getAllSoftClassesWithParam()
    {
        $name = $this->ci_instance->class_model->getAllSoftClasses($this->username, $this->searchletter, $this->default_include, $this->default_exclude, 10, 1);
        $this->assertTrue(is_array(json_decode($name)), "Must be a valid set of class");
    }

}

?>