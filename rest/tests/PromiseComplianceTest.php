<?php

require_once "RestBaseTest.php";

class PromiseComplianceTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAll()
    {
        try
        {
            $jsonArray = $this->getResults('/promise/compliance');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test promise compliance with host key parameter 
     */
    public function testWithHostKey()
    {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->getResults('/promise/compliance?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== "$hostKey")
            {
                $this->fail("different host key found in data found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test promise compliance with handle 
     */
    public function testWithHandle()
    {

        $handle = "check_valid_update";
        $jsonArray = $this->getResults('/promise/compliance?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(2, $jsonArray);
        foreach ((array) $jsonArray as $data)
        {
            if ($data['handle'] !== "$handle")
            {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test promise compliance with context 
     */
    public function testWithContext()
    {
        $context = "10_0_0_153";
        $jsonArray = $this->getResults('/promise/compliance?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== $this->hostB)
            {
                $this->fail("different host key found in data, found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test promise compliance with state 
     */
    public function testWithState()
    {
        $jsonArray = $this->getResults('/promise/compliance?state=repaired');
        $this->assertValidJson($jsonArray);
        $this->assertEquals(3, sizeof($jsonArray));
        foreach ((array) $jsonArray as $entry)
        {
            $this->assertEquals('repaired', $entry['state']);
        }
    }

    /**
     * Invalid state should get a bad request
     */
    public function testWithInvalidState()
    {
        try
        {
            $jsonArray = $this->getResults('/promise/compliance?state=snookie');
        }
        catch (Pest_BadRequest $e)
        {
            return;
        }
        $this->fail('Should get BadRequest');
    }
}
