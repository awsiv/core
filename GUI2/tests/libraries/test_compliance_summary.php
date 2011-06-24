<?php

class test_compliance_summary extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        
    }

    public function tearDown() {
        
    }

    public function test_environmentList() {
        $array = cfpr_environments_list();
        $this->dump($array);
        $this->assertTrue(is_array($array), "Must return an array");
    }

    public function test_complianceSummaryGraph() {
        $data = cfpr_compliance_summary_graph(NULL);
       
        $array = json_decode(utf8_encode($data), true);
        $this->dump($data);
        $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Must return an array of the graph data");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */