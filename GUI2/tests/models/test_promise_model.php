<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
class test_promise_model extends CodeIgniterUnitTestCase {
public $username="sudhir";
public $handle="change_management_files_watch_files"; //"change_management_files_watch_files"
    public function __construct() {
        parent::__construct();
    }
    
    public function setUp() {
        $this->_ci->load->model('promise_model');
    }

    public function tearDown() {
        
    }
    
    public function test_getBundleByPromiseHandle() {
        $data = $this->_ci->promise_model->getBundleByPromiseHandle($this->username,$this->handle);
        $this->dump($data);
        $this->assertTrue(is_string($data), "Should Return a valid string");
    }

    public function test_getPromiseDetails() {
        $data = $this->_ci->promise_model->getPromiseDetails($this->username,$this->handle);
        $this->dump($data);
        $this->assertTrue(is_array($data), "Should Return a valid array");
    }
}
?>
