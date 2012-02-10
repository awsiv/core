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
}
