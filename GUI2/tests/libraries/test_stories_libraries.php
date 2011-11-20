<?php
class test_stories_libraries extends CodeIgniterUnitTestCase {

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
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->dump($array);
        
    }
    
    public function test_api_function_exist(){
        $this->assertTrue(function_exists("cfcon_get_story_by_name"),"The function cfpr_get_story available or not");
        $this->assertTrue(function_exists("cfcon_get_story_by_id"),"The function cfpr_get_story_by_id available or not");
    }
    
    public function test_cfpr_get_story_by_name_result(){
        $data=cfcon_get_story_by_name('webserver');
        $this->dump($data);
        $this->validateJson($data);
    }
    
  
}
?>
