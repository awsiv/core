<?php

class test_knowledge_map_model extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('knowledge_model');
        $this->knowledge_model = $this->_ci->knowledge_model;
        $this->username = 'admin';
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
        $pid = $this->knowledge_model->getPidForTopic($this->username, "", "system policy");
        $this->assertTrue(is_numeric($pid), "There must be some pid returned, pid returned is :: $pid");
    }

    public function test_getValidJsonforKnowledgeMapPid212() {
        $pid = 212;
        try {
            $gdata = $this->knowledge_model->getKnowledgeView($this->username, $pid);
            $this->validJson($gdata);
            $this->dump($gdata);
        } catch (Exception $e) {
            $this->fail('Exception throwed :: '.$e->getMessage());
        }
    }

    public function test_getValidJsonforTopicHits() {
        $pid = $this->knowledge_model->getPidForTopic($this->username, "", "system policy");

        $array = $this->knowledge_model->showTopicHits($this->username, $pid);

        $this->assertTrue(is_array($array), "Should Return a valid array");
        $this->dump($array);
    }

    public function test_getValidJsonforTopicLeads() {
        $pid = $this->knowledge_model->getPidForTopic($this->username, "", "system policy");
        $array = $this->knowledge_model->showTopicLeads($this->username, $pid);
        $this->assertTrue(is_array($array), "Should Return a valid array");
    }

    public function test_getValidJsonforTopicCategory() {
        $pid = $this->knowledge_model->getPidForTopic($this->username, "", "system policy");
        $array = $this->knowledge_model->showTopicCategory($this->username, $pid);
        $this->assertTrue(is_array($array), "Should Return a valid array");
    }

    public function test_getValidJsonForKnowledgeSearch() {
        $search = 'update'; //test string
        $data = $this->knowledge_model->searchTopics($this->username, $search);
        $this->assertTrue(is_array($data), "Should Return a valid array");
    }

    function test_manualKnowledgeMapSearch() {
        $pid = $this->knowledge_model->getPidForTopic($this->username, "any", "manuals");
        $data = $this->knowledge_model->showTopicHits($this->username, $pid);
        $this->assertTrue(is_array($data), "Should Return a valid array");
    }

    function test_topicDetails() {
        $pid = $this->knowledge_model->getPidForTopic($this->username, "promisers", '"/var/cfengine/bin/cf-execd"');
        try {
            $data = $this->knowledge_model->showTopics($this->username, $pid);
        } catch (Exception $e) {
            $this->dump($e);
        }
        $this->assertTrue(is_array($data), "Should Return a valid array");
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */