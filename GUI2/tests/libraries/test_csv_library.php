<?php

class test_csv extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->library('cf_csv');
        $this->tempDir = "./tmp/";
        $this->csvFileName = "tempcsv.csv";
        $this->filePath = $this->tempDir . $this->csvFileName;
        $this->header = array("H1", "H2","H3");
        $this->data = array(
            array("R11", "R12","R13"),
            array("R21", "R22","R23"),
            array("R31", "R32", "R33")
        );
    }

    public function tearDown() {
        unlink($this->filePath);
    }

    function testCsvFile() {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader($this->header);
        $this->_ci->cf_csv->setData($this->data);
        $this->_ci->cf_csv->Output(null, 'F');
        $content = file_get_contents($this->filePath, true);
        $output = "H1,H2,H3\nR11,R12,R13\nR21,R22,R23\nR31,R32,R33";
        $this->assertEqual(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }
    
    function testCsvFileWithEmptyHeader() {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader(array());
        $this->_ci->cf_csv->setData($this->data);
        $this->_ci->cf_csv->Output(null, 'F');
        $content = file_get_contents($this->filePath, true);
        $output = "R11,R12,R13\nR21,R22,R23\nR31,R32,R33";
        $this->assertEqual(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }

    
    function testCsvFileWithEmptyHeaderAndData() {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader(array());
        $this->_ci->cf_csv->setData(array());
        $this->_ci->cf_csv->Output(null, 'F');
        $content = file_get_contents($this->filePath, true);
        $output = "";
        $this->assertEqual(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */