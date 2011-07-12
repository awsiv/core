<?php

class test_search_save_model extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('search_save_model');
        $username = 'unit-test';
        $url = 'http://test/test/1/23/4/31';
        $label = 'test label';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $search = $this->_ci->search_save_model->insert($data);
    }

    public function tearDown() {
        
        // delete all the test inserts
          $filter = array('username' => 'unit-test');
          $return = $this->_ci->search_save_model->delete($filter);
        
        
        
    }

    public function test_insertSavedSearch() {

        $username = 'unit-test';
        $url = 'http://test/test/1/23/4/31';
        $label = 'test label';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $search = $this->_ci->search_save_model->insert($data);
        $this->assertIsA($search, 'CF_Search');
    }

    public function test_getAllSavedSearchMustReturnArray() {

        $filter = array();
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->dump($result);
        $this->assertTrue(is_array($result));
    }

    public function test_getAllSavedSearchWithFilterMustReturnArray() {

        $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->assertTrue(is_array($result));
    }

    public function test_getAllSavedSearchWithFilterMustReturnArrayWithValidObject() {

         $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->dump($result);
        $this->assertIsA($result[0], 'CF_Search');
    }

    public function test_getAllSavedSearchWithFilterUsername() {

        $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->dump($result);
        $username = $result[0]->getUsername();
        $this->assertEqual($username, 'unit-test');
    }
    
    
    public function test_emptyLabelInsert() {
        $username = 'unit-test';
        $url = 'http://test/test/1/23/4/31';
        $label = '';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $this->dump($data);
        $search = $this->_ci->search_save_model->insert($data);
        $this->assertFalse($search,'Must fail with empty label');
        
    }
    
    public function test_emptyLabelUsername() {
        $username = '';
        $url = 'http://test/test/1/23/4/31';
        $label = 'test';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $this->dump($data);
        $search = $this->_ci->search_save_model->insert($data);
        $this->assertFalse($search,'Must fail with empty label');
        
    }
    
    public function test_SearchByReportTitle() {
        $filter = array('reportType' => 'test report');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->dump($result);
        foreach ($result as $obj) {
        $report = $obj->getReportType();
        $this->assertEqual('test report', $report);
        
        }
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */