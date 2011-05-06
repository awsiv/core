<?php

class test_promise_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();

    }

    public function setUp() {
        $this->testHandle = "cfengine_php_mod_cfmod_ini_ubuntu10";
    }

    public function tearDown() {

    }

    public function test_summarizePromise() {
        $promise = cfpr_summarize_promise($this->testHandle);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");

      }
      
      
      public function test_listHandlesForBundles(){
        $b = cfpr_get_promise_bundle(  $this->testHandle );
        $promise = cfpr_list_handles_for_bundle($b, "agent", false);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
          
      }
      
       public function test_allHandlePromiser(){
        $p = cfpr_get_promiser(  $this->testHandle );
        $promise = cfpr_list_handles($p, "", false);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
          
      }
      
       public function test_allHandleByType(){
        $type = cfpr_get_promise_type($this->testHandle);
        $promise = cfpr_list_handles("", $type, false);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
          
      }


}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */