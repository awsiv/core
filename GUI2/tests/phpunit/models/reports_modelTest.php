<?php
class test_reports_model extends CI_TestCase
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

        $this->load->model('report_model');
    }
    
    public function test_get_all_bundles()
    {
       
    }
}
?>
