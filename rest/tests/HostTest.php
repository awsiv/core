<?php

require_once "RestBaseTest.php";

class HostTest extends RestBaseTest
{

    public function testAllHost()
    {
        try
        {
            $jsonArray = $this->pest->get('/host');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(2, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostByIP()
    {
        try
        {
            $jsonArray = $this->pest->get("/host?ip=$this->hostA_ip");
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
            $this->assertEquals($this->hostA, $jsonArray[0], "got key :: $jsonArray[0]");
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostByName()
    {
        try
        {
            $jsonArray = $this->pest->get('/host?hostname=hostA');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
            $this->assertEquals($this->hostA, $jsonArray[0], "got key :: $jsonArray[0]");
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostByInvalidIP()
    {
        try
        {
            $ip = '11230.0123.03123.2923';
            $jsonArray = $this->pest->get('/host?ip=' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostByInvalidName()
    {
        try
        {
            $hostname = 'hub.test.invalidname';
            $jsonArray = $this->pest->get('/host?hostname=' . $hostname);
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
