<?php

require_once "RestBaseTest.php";

class ContextTest extends RestBaseTest
{

    public function testAllContext()
    {
        try
        {
            $jsonArray = $this->getResults('/context');
            $this->assertValidJson($jsonArray);
            $this->assertFalse(empty($jsonArray), "Should not be empty value");
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testWithHostKey()
    {

        $hostKey = "SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
        $jsonArray = $this->getResults('/context?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertEquals(50, sizeof($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== "$hostKey")
            {
                $this->fail("different host key found in data found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }

    public function testWithInvalidKey()
    {

        $hostKey = "SHA=33736d45041e2a9407be8cf449a";
        $jsonArray = $this->getResults('/context?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertEquals(0, sizeof($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== "$hostKey")
            {
                $this->fail("different host key found in data found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }

    public function testContextByName()
    {
        try
        {
            $jsonArray = $this->getResults('/context?name=io_writtendata_.*' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(2, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
