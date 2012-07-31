<?php

class test_vitals_model extends CI_BaseTestCase
{

    private $ci_obj;
    private $username = "admin";
    private $hostkey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";


    private function validateJson($json)
    {

        $array = json_decode(utf8_encode($json), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertEquals($retValue, 0, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function setUp()
    {
        parent::setUp();
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->_ci = new StdClass;



        // Fix get_instance()
        $this->ci_instance($this->_ci);

        $this->load->model('vitals_model');
    }

    public function test_model_exist()
    {
        $this->ci_set_core_class('model', 'CI_Model');


        $this->assertNull($this->load->model('vitals_model'));

        // Was the model class instantiated.
        $this->assertTrue(class_exists('vitals_model'));
    }

    public function testVitalList()
    {
        $graphdata = $this->_ci->vitals_model->getVitalsList($this->username, $this->hostkey);
        $this->assertTrue(is_array($graphdata) && !empty($graphdata['obs']));
    }

    public function test_vital_magnified()
    {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsMagnifiedViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_vital_week()
    {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsWeekViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_vital_year()
    {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsYearViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_vital_histogram()
    {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsHistogramViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_vital_histogram_analysis()
    {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsHistogramAnalysis($this->username, $this->hostkey, $observables);
        $this->assertTrue(is_array($test) && !empty($test));
    }

   public function test_invalid_vital_magnified()
    {
        $observables = 'invalid_id';
        $test = $this->_ci->vitals_model->getVitalsMagnifiedViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_invalid_vital_week()
    {
        $observables = 'invalid_id';
        $test = $this->_ci->vitals_model->getVitalsWeekViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_invalid_vital_year()
    {
        $observables = 'invalid_id';
        $test = $this->_ci->vitals_model->getVitalsYearViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

    public function test_invalid_vital_histogram()
    {
        $observables = 'invalid_id';
        $test = $this->_ci->vitals_model->getVitalsHistogramViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
    }

}