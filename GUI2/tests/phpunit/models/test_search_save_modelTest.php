<?php

class test_search_save_modelTest extends CI_TestCase
{

    private $_ci;

    public function set_up()
    {

        $config = $this->getMock('CI_Config', NULL, array(), '', FALSE);




        // Add the mock  to our stdClass
        // set predictable config values
        $this->ci_set_config(array(
            'index_page' => 'index.php',
            'base_url' => 'http://example.com/',
            'subclass_prefix' => 'MY_'
        ));



        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();

        // mock up a ci instance
        $this->_ci = new StdClass;

        // Fix get_instance()
        $this->ci_instance($this->_ci);
        $this->ci_instance_var('config', $config);


        $this->load->model('search_save_model');
        $this->load->library('mongo_db');


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

    public function tearDown()
    {

        // delete all the test inserts
        $filter = array('username' => 'unit-test');
        $return = $this->_ci->search_save_model->delete($filter);
    }

    public function test_insertSavedSearch()
    {

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
        $this->assertInstanceOf('CF_Search', $search);
    }

    public function test_getAllSavedSearchMustReturnArray()
    {

        $filter = array();
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->assertTrue(is_array($result));
    }

    public function test_getAllSavedSearchWithFilterMustReturnArray()
    {

        $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->assertTrue(is_array($result));
    }

    public function test_getAllSavedSearchWithFilterMustReturnArrayWithValidObject()
    {

        $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $this->assertInstanceOf('CF_Search', $result[0]);
    }

    public function test_getAllSavedSearchWithFilterUsername()
    {

        $filter = array('username' => 'unit-test');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        $username = $result[0]->getUsername();
        $this->assertEquals($username, 'unit-test');
    }

    public function test_emptyLabelInsert()
    {
        $username = 'unit-test';
        $url = 'http://test/test/1/23/4/31';
        $label = '';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $search = $this->_ci->search_save_model->insert($data);
        $this->assertFalse($search, 'Must fail with empty label');
    }

    public function test_emptyLabelUsername()
    {
        $username = '';
        $url = 'http://test/test/1/23/4/31';
        $label = 'test';
        $report_title = 'test report';
        $data = array('username' => $username,
            'url' => $url,
            'reportType' => $report_title,
            'label' => $label,
            'date' => time());
        $search = $this->_ci->search_save_model->insert($data);
        $this->assertFalse($search, 'Must fail with empty label');
    }

    public function test_SearchByReportTitle()
    {
        $filter = array('reportType' => 'test report');
        $result = $this->_ci->search_save_model->get_all_search($filter);
        foreach ($result as $obj)
        {
            $report = $obj->getReportType();
            $this->assertEquals('test report', $report);
        }
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */