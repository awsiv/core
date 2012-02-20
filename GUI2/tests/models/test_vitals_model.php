<?php

class test_vitals_model extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('vitals_model');
        $this->username = 'admin';
        $this->hostkey = cfpr_hub_key();
    }

    public function tearDown() {
        
    }

    private function validateJson($json) {

        $array = json_decode(utf8_encode($json), true);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($array);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_vitalsList() {
        $graphdata = $this->_ci->vitals_model->getVitalsList($this->username, $this->hostkey);
        $this->assertTrue(is_array($graphdata) && !empty($graphdata['obs']));
        $this->dump($graphdata);
    }
    
    public function test_vital_magnified() {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsMagnifiedViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
        $this->dump($test);
    }
    
      public function test_vital_week() {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsWeekViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
        $this->dump($test);
    }

      public function test_vital_year() {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsYearViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
        $this->dump($test);
    }
    
    public function test_vital_histogram() {
        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsHistogramViewJson($this->username, $this->hostkey, $observables);
        $this->validateJson($test);
        $this->dump($test);
    }
    
    
    
    

    public function test_vital_analysis_magnified() {

        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsMagnifiedAnalysis($this->username, $this->hostkey, $observables);
        $this->assertTrue(is_array($test) && !empty($test));
        $this->dump($test);
    }

    public function test_vital_analysis_week() {

        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsWeekAnalysis($this->username, $this->hostkey, $observables);
        $this->assertTrue(is_array($test) && !empty($test));
        $this->dump($test);
    }

    public function test_vital_analysis_year() {

        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsYearAnalysis($this->username, $this->hostkey, $observables);
        $this->assertTrue(is_array($test) && !empty($test));
        $this->dump($test);
    }

    public function test_vital_analysis_histogram() {

        $observables = 'mem_total';
        $test = $this->_ci->vitals_model->getVitalsHistogramAnalysis($this->username, $this->hostkey, $observables);
        $this->assertTrue(is_array($test) && !empty($test));
        $this->dump($test);
    }
    
    public function test_latest_update() {
        $timestamp = $this->_ci->vitals_model->getVitalsLastUpdate($this->username, $this->hostkey);
        $this->assertTrue(is_numeric($timestamp) && (int)$timestamp == $timestamp);
        $this->dump($timestamp);
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */