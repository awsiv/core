<?php

class test_csv extends CI_TestCase
{

    public function __construct()
    {
        parent::__construct();
    }

    public function setUp()
    {
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->_ci = new StdClass;

        // Fix get_instance()
        $this->ci_instance($this->_ci);


        // Add the mock  to our stdClass
        // set predictable config values

        $config = $this->getMock('CI_Config', NULL, array(), '', FALSE);
        $this->ci_instance_var('config', $config);
        $this->ci_set_config(array(
            'subclass_prefix' => 'Cf_'
        ));

        $this->load->library('cf_csv');
      
        $this->header = array("H1", "H2", "H3");
        $this->data = array(
            array("R11", "R12", "R13"),
            array("R21", "R22", "R23"),
            array("R31", "R32", "R33")
        );

        vfsStreamWrapper::register();
        vfsStreamWrapper::setRoot(new vfsStreamDirectory('testdir'));
        $this->tempDir = vfsStream::url('testdir');
        $this->csvFileName = "testcsv.csv";
        $this->filePath = $this->tempDir . DIRECTORY_SEPARATOR . $this->csvFileName;
    }

    public function tearDown()
    {
        unlink($this->filePath);
    }

    function testCsvFile()
    {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader($this->header);
        $this->_ci->cf_csv->setData($this->data);
        $this->_ci->cf_csv->Output(null, 'F');
        $this->assertTrue(vfsStreamWrapper::getRoot()->hasChild($this->csvFileName));
        $content = file_get_contents($this->filePath);
        $output = "H1,H2,H3\nR11,R12,R13\nR21,R22,R23\nR31,R32,R33";
        $this->assertEquals(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }

    function testCsvFileWithEmptyHeader() {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader(array());
        $this->_ci->cf_csv->setData($this->data);
        $this->_ci->cf_csv->Output(null, 'F');
        $content = file_get_contents($this->filePath);
        $output = "R11,R12,R13\nR21,R22,R23\nR31,R32,R33";
        $this->assertEquals(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }


    function testCsvFileWithEmptyHeaderAndData() {
        $this->_ci->cf_csv->setFileName($this->filePath);
        $this->_ci->cf_csv->setHeader(array());
        $this->_ci->cf_csv->setData(array());
        $this->_ci->cf_csv->Output(null, 'F');
        $content = file_get_contents($this->filePath);
        $output = "";
        $this->assertEquals(trim($output), trim($content), "both should be equal ,got content:-- $content");
    }
}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */