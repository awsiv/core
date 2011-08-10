<?php

class test_replication extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
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
        return $array;
        
    }

    public function test_replicaStatus() {
        $ret = cfpr_replica_status();
       $array =  $this->validateJson($ret);
    }
    
    public function test_hub_master(){
        $ret=cfpr_get_hub_master();
        $this->assertEqual($ret, 'am_hub_master', "This is A hub master");
    }
    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */