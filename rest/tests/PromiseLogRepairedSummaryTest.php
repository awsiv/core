<?php

require_once "RestBaseTest.php";

class PromiseLogRepairedSummaryTest extends RestBaseTest
{

    /**
     * test valid json response
     */
    public function testAll()
    {
        try
        {
            $jsonArray = $this->getResults('/promise/log/repaired/summary');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test Promise Log Repaired summary  with host key parameter
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->getResults('/promise/log/repaired/summary?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test Promise Log Repaired summary with handle
     */
    public function testWithHandle()
    {

        $handle = "garbage_collection_files_tidy_outputs";
        $jsonArray = $this->getResults('/promise/log/repaired/summary?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(4, $jsonArray);
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
        $jsonArray = $this->getResults('/promise/log/repaired/summary?context=' . $context);
        $this->assertValidJson($jsonArray);
        $handle = "garbage_collection_files_tidy_outputs";
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['handle'] !== "$handle")
            {
                $this->fail("different handle  found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }

}
