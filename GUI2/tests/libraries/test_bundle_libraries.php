<?php

class test_bundle_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->bundle = "main";
        $this->type = "";
    }

    public function tearDown() {

    }

    public function test_allBundles() {
        $list = cfpr_bundle_list_all();
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_handlesForBundles() {
        $list = cfpr_list_handles_for_bundle($this->bundle, $this->type, False);
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }
    
     public function test_argsForBundles() {
        $list = cfpr_get_args_for_bundle('set_variable_values2', cfpr_get_bundle_type('set_variable_values2'));
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }
    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */
