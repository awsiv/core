<?php

require_once "RestBaseTest.php";

class PromiseLogRepairedTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAll()
    {
        try
        {
            $jsonArray = $this->pest->get('/promise/log/repaired/');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test Promise Log Repaired   with host key parameter 
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->pest->get('/promise/log/repaired?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test Promise Log Repaired summary with handle 
     */
    public function testPromiseLogRepairedWithHandle()
    {

        $handle = "garbage_collection_files_tidy_outputs";
        $jsonArray = $this->pest->get('/promise/log/repaired?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data)
        {
            if ($data['handle'] !== "$handle")
            {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test  Promise Log Repaired summary with context 
     */
    public function testWithContext()
    {

        $context = "10_0_0_150";
        $jsonArray = $this->pest->get('/promise/log/repaired?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== $this->hostA)
            {
                $this->fail("different hostkey  found in data, found :: " . $data['hostkey'] . " Expected :: " . $this->hostA);
            }
        }
    }

}
