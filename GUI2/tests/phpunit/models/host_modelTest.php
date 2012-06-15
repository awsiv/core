<?php

class test_host_model extends CI_TestCase
{

    private $hostkey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
    private $hostname = "hub.test.cfengine.com";
    private $hostip = "10.0.0.29";
    private $username = "admin";

    public function setUp()
    {
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->_ci = new StdClass;

        // Fix get_instance()
        $this->ci_instance($this->_ci);
        $this->load->model('host_model');
        $this->includes = array();
        $this->excludes = array();
    }


    public function test_getHostName()
    {
        $name = $this->_ci->host_model->getHostName($this->username, $this->hostkey);
        $this->assertNotNull($name);
        $this->assertEquals($this->hostname,$name);
    }

    public function test_getHostIp()
    {
        $ip = $this->_ci->host_model->getHostIp($this->username, $this->hostkey);
        $this->assertNotNull($ip);
        $this->assertEquals($ip,$this->hostip);
    }

    public function test_getLastUpdate()
    {
        $date = $this->_ci->host_model->getLastUpdate($this->username, $this->hostkey);
        $this->assertTrue(is_numeric($date), "Must be ablid time stamp");
    }

    public function test_getHostVariable()
    {
        $val = $this->_ci->host_model->getHostVariable($this->username, $this->hostkey, "mon", "av_loadavg");
        $this->assertTrue(is_string($val));
    }

    public function test_getNetWorkSpeed()
    {
        $val = $this->_ci->host_model->getNetworkSpeed($this->username, $this->hostkey);
        $this->assertTrue(is_array($val));
    }

    public function test_getCompliancelist()
    {
        $val = $this->_ci->host_model->getComplianceList($this->username, $this->includes, $this->excludes, 15, 1);
        $this->assertTrue(is_array($val), "should return a valid array");
        $this->assertNotEquals(count($val['data']), 0, "should not be empty");
    }

    public function test_getHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, '', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of all host should be a number ');
    }

    public function test_getRedHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'red', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of red host should be a number ');
    }

    public function test_getYellowHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'yellow', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of red host should be a number ');
    }

    public function test_getGreenHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'green', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of red host should be a number ');
    }

    public function test_getBlueHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'blue', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of red host should be a number ');
    }

    public function test_getBlackHostCount()
    {
        $ret = $this->_ci->host_model->getHostCount($this->username, 'black', $this->includes, $this->excludes);
        $this->assertTrue(is_numeric($ret), 'Count of red host should be a number ');
    }

    public function test_getHostByName()
    {
        $ret = $this->_ci->host_model->getHostByName($this->username, '.*');
        $this->assertTrue(is_array($ret), 'hostname list ');
    }

}

?>
