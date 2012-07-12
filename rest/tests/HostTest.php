<?php

require_once "RestBaseTest.php";

class HostTest extends RestBaseTest
{

    public function testAllHost()
    {
        try
        {
            $jsonArray = $this->getResults('/host');
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
            $jsonArray = $this->getResults("/host?ip=$this->hostA_ip");
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
            $jsonArray = $this->getResults('/host?hostname=hostA');
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
            $jsonArray = $this->getResults('/host?ip=' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostByContext()
    {
        try
        {
            $jsonArray = $this->getResults('/host?context=PK_SHA_30565.*' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
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
            $jsonArray = $this->getResults('/host?hostname=' . $hostname);
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostInFullTimeframe()
    {
        try
        {
            $from = 0;
            $to = time(NULL);
            $jsonArray = $this->getResults('/host?from=' . $from . '&to=' . $to);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(2, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testHostInTimeframe()
    {
        try
        {
            $from = 1328000000;
            $to = time(NULL);
            $jsonArray = $this->getResults('/host?from=' . $from . '&to=' . $to);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
