<?php

class test_bundles_model extends CI_TestCase
{

    private $ci_obj;
    private $username = "admin";

    public function set_up()
    {
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->ci_obj = new StdClass;



        // Fix get_instance()
        $this->ci_instance($this->ci_obj);

        $this->load->model('bundle_model');
    }

    public function test_model_exist()
    {
        $this->ci_set_core_class('model', 'CI_Model');


        $this->assertNull($this->load->model('bundle_model'));

        // Was the model class instantiated.
        $this->assertTrue(class_exists('bundle_model'));
    }

    public function test_get_all_bundles()
    {
        $bundles = $this->ci_obj->bundle_model->getAllBundles($this->username);
        $this->assertNotEmpty($bundles);
    }

    public function test_get_bundle_arguments_with_no_arguments()
    {
        $args = $this->ci_obj->bundle_model->getBundleArguments($this->username, 'agent', 'main');
        $this->assertEmpty($args);
    }

    public function test_get_bundle_arguments_with_arguments()
    {
        $args = $this->ci_obj->bundle_model->getBundleArguments($this->username, 'agent', 'cronjob');
        $this->assertNotEmpty($args);
        $expectedArray = array("commands", "user", "hours", "mins");
        $this->assertEquals($args, $expectedArray);
    }

    public function test_get_bundle_class_used()
    {
        $args = $this->ci_obj->bundle_model->getBundleClassesUsed($this->username, 'agent', 'cronjob');
        $this->assertNotEmpty($args);
        $expectedArray = array(array(0, "SuSE"), array(0, "any"), array(0, "changed_crontab"), array(0, "fedora"), array(0, "redhat"), array(0, "windows"));
        $this->assertEquals($args, $expectedArray);
    }

    public function test_get_bundle_class_used_with_wrong_class_name()
    {
        $args = $this->ci_obj->bundle_model->getBundleClassesUsed($this->username, 'wrong-type', 'wrong-name');
        $this->assertEmpty($args);
    }


    public function test_get_bundle_usage()
    {
        $args = $this->ci_obj->bundle_model->getBundleListByUsage($this->username, 'main');
        $this->assertEmpty($args);
    }






}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */