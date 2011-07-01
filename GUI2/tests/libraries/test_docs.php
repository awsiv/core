<?php

class test_docs extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        
    }

    public function tearDown() {
        
    }

    private function validateJson($json) {

        $array = json_decode(utf8_encode($json), true);
        $retValue = json_last_error();

        $this->dump($array);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($json);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_docsList() {
        $data = cfpr_list_documents(NULL);
        $this->validateJson($data);
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */