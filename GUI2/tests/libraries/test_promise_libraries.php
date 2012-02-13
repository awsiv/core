<?php

class test_promise_libraries extends CodeIgniterUnitTestCase {

   
    
    public function __construct() {
        parent::__construct();

    }

    public function setUp() {
        //$this->testHandle = "cfengine_php_mod_cfmod_ini_ubuntu10";
        $this->testHandle = "garbage_collection_files_tidy_outputs";
        $this->username   = 'admin'; // set username who will "run" test - we need this for RBAC
    }

    public function tearDown() {

    }

    public function test_summarizePromise() {
        $promise = cfpr_promise_details($this->username, $this->testHandle);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");

      }
      
      public function testFailingHandle(){
        $promise = cfpr_promise_details($this->username, 'apache2_nova_default_insert_lines_site_available_default');
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
      }


      
      
      
      public function test_listHandlesForBundles(){
        $b = cfpr_bundle_by_promise_handle($this->username, $this->testHandle );
        $promise = cfpr_promise_list_by_bundle($this->username, "agent", $b);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
          
      }
      
    
       public function test_allHandlePromiser(){
        $promise_details = sanitycheckjson(cfpr_promise_details($this->username, $this->testHandle), TRUE);
        $p = $promise_details['promiser'];
        
        $promise = cfpr_promise_list_by_promiser($this->username, $p);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
          
      }
      
       public function test_allHandleByType() {
        $promise_details = sanitycheckjson(cfpr_promise_details($this->username, $this->testHandle), TRUE);
        $type = $promise_details['promise_type'];

        $promise = cfpr_promise_list_by_promise_type($this->username, $type);
        $this->dump($promise);
        $array = json_decode(utf8_encode($promise), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");          
      }


}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */
