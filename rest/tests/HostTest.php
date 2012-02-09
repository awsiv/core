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
            $ip = '10.0.0.150';
            $jsonArray = $this->pest->get('/host?ip=' . $ip);
            $this->assertValidJson($jsonArray);
            $expectedKey = "SHA=305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283";
            $this->assertEquals($expectedKey, $jsonArray[0], "got key :: $jsonArray[0]");
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
            $hostname = 'hub.test.cfengine.com';
            $jsonArray = $this->pest->get('/host?hostname=' . $hostname);
            $this->assertValidJson($jsonArray);
            $expectedKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
            $this->assertEquals($expectedKey, $jsonArray[0], "got key :: $jsonArray[0]");
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
