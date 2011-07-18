<?php

class test_vitals extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->hostkey = 'SHA=bec807800ab8c723adb027a97171ceffb2572738e492a2d5949f3dc82371400e';
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
        $graphdata = cfpr_vitals_list($this->hostkey);
        $this->validateJson($graphdata);
    }
    
    public function test_vital_analysis_magnified() {
        
        $observables = 'mem:total';
        $test = cfpr_vitals_analyse_magnified($this->hostkey, $observables);
        $this->dump($test);
        $this->validateJson($test);
        
        
    }
     public function test_vital_analysis_week() {
        
        $observables = 'mem:total';
        $test = cfpr_vitals_analyse_week($this->hostkey, $observables);
        $this->dump($test);
        $this->validateJson($test);
        
        
    }
     public function test_vital_analysis_year() {
        
        $observables = 'mem:total';
        $test = cfpr_vitals_analyse_year($this->hostkey, $observables);
        $this->dump($test);
        $this->validateJson($test);
        
        
    }
    public function test_vital_analysis_histogram() {
        
        $observables = 'mem:total';
        $test = cfpr_vitals_analyse_histogram($this->hostkey, $observables);
        $this->dump($test);
        $this->validateJson($test);
        
        
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */