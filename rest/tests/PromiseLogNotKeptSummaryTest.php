<?php

require_once "RestBaseTest.php";

class PromiseLogNotKeptSummaryTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAllPromiseLogNotKeptSummary()
    {
        try
        {
            $jsonArray = $this->pest->get('/promise/log/not-kept/summary');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test with host key parameter 
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test PromiseLogNotKept with handle 
     */
    public function testWithHandle()
    {

        $handle = "cfengine_correct_cftwin_files_libtwin";
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(1, $jsonArray);
        foreach ((array) $jsonArray as $data)
        {
            if ($data['handle'] !== "$handle")
            {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
        $this->assertEquals(4, $jsonArray[0]['count']);
    }

    /**
     * Test  PromiseLogNotKept with context 
     */
    public function testWithContext()
    {

        $context = "10_0_0_150";
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertCount(2, $jsonArray);
        $this->assertEquals(4, $jsonArray[0]['count']);
    }

}
