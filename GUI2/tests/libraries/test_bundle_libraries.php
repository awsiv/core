<?php

class test_bundle_libraries extends CodeIgniterUnitTestCase {
    
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->bundle = "main";
        $this->type = "";
        $this->username = 'admin'; // set username who will "run" test - we need this for RBAC
    }

    public function tearDown() {

    }

    public function test_allBundles() {
        $list = cfpr_bundle_list_all($this->username);
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_handlesForBundles() {
        $list = cfpr_promise_list_by_bundle($this->username, $this->type, $this->bundle); 
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }
    
     public function test_argsForBundles() {
        $list = cfpr_bundle_arguments($this->username, cfpr_get_bundle_type('set_variable_values2'), 'set_variable_values2');
        $this->dump($list);
        $array = json_decode(utf8_encode($list), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }
    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */
