<?php

require_once "RestBaseTest.php";

class HostIdSeenTest extends RestBaseTest
{

    /**
     * Check for valid json data 
     */
    public function testhostIdSeen()
    {
        try
        {
            $jsonArray = $this->pest->get("/host/$this->hostA/seen");
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
            $this->assertEquals($this->hostB, $jsonArray[0]['hostkey']);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Checks the format for invalid key and empty result
     */
    public function testhostIdSeenWithInvalidKey()
    {
        try
        {
            $id = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697c";
            $jsonArray = $this->pest->get("/host/$id/seen");
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Checks the remote context parameter
     * Matches the data format (array keys) that are returned and expected, doesnot check for the values 
     */
    public function testhostIdSeenWithRemoteContext()
    {
        try
        {
            $context = "10_0_0_153";
            $id = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
            $jsonArray = $this->pest->get("/host/$id/seen?context=$context");
            $this->assertValidJson($jsonArray);
            $expectedArray = array(
                "hostkey" => "SHA=db6f6be353bb61e3e02dcae40c1d9c9d89a5e60edd42fabd8ee1da44552efda6",
                "last-seen" => 288,
                "average" => 288,
                "stdv" => 180
            );
            $this->assertFalse(empty($jsonArray), "Result should not be empty");
            if (isset($jsonArray[0]))
            {
                $this->assertTrue($this->array_keys_exists($jsonArray[0], array_keys($expectedArray)), "Returned data not equal to expected data.");
            }
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
