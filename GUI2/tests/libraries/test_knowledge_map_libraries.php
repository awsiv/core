<?php

class test_knowledge_map_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        
    }

    public function tearDown() {
        
    }

    private function validJson($data) {
        $this->dump($data);
        $array = json_decode(utf8_encode($data), true);
        $retValue = json_last_error();
        $this->dump($array);

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getDefaultPid() {
        $pid = cfpr_get_pid_for_topic("", "system policy");
//        $this->dump($pid);
        $this->assertTrue($pid, "There must be some pid returned, pid returned is :: $pid");
    }

    public function test_getValidJsonforKnowledgeMapPid212() {
        $pid = 212;
        $gdata = cfpr_get_knowledge_view($pid, '');
        $array = json_decode(utf8_encode($gdata), true);
        $retValue = json_last_error();
        $this->dump($gdata);

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getValidJsonforKnowledgeMap() {
        $pid = cfpr_get_pid_for_topic("", "system policy");
        $gdata = cfpr_get_knowledge_view($pid, '');
        $array = json_decode(utf8_encode($gdata), true);
        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        //   $this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getValidJsonforTopicHits() {
        $pid = cfpr_get_pid_for_topic("", "system policy");
        $gdata = cfpr_show_topic_hits($pid);
        $array = json_decode(utf8_encode($gdata), true);

        $retValue = json_last_error();

        $this->assertTrue(is_array($array), "Should Return a valid array");
        //$this->dump($gdata);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getValidJsonforTopicLeads() {
        $pid = cfpr_get_pid_for_topic("", "system policy");
        $gdata = cfpr_show_topic_leads($pid);
        $array = json_decode(utf8_encode($gdata), true);

        $retValue = json_last_error();

        $this->dump($gdata);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getValidJsonforTopicCategory() {
        $pid = cfpr_get_pid_for_topic("", "system policy");
        $gdata = cfpr_show_topic_category($pid);
        $array = json_decode(utf8_encode($gdata), true);

        $retValue = json_last_error();
        $this->dump($gdata);
        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
    }

    public function test_getValidJsonForKnowledgeSearch() {
        $search = 'update'; //test string
        $data = cfpr_search_topics($search, true);
        $this->validJson($data);
    }

    function test_manualKnowledgeMapSearch() {
        $pid = cfpr_get_pid_for_topic("any", "manuals");
        $data = cfpr_show_topic_hits($pid);
        $this->validJson($data);
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */