<?php

require_once "RestBaseTest.php";

class HostIdTest extends RestBaseTest
{

    /**
     * Checks for valid json response 
     */
    public function testhostID()
    {
        try
        {
            $jsonArray = $this->pest->get("/host/$this->hostA");
            $this->assertValidJson($jsonArray);
            $this->assertEquals("hostA", $jsonArray["name"]);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }
    
    /**
     * Get a 404 back for invalid host id
     */
    public function testInvalidId()
    {
        try
        {
            $jsonArray = $this->pest->get("host/$this->hostA");
        }
        catch (Pest_NotFound $e)
        {
            return;
        }
        $this->fail('Found resource');
    }

    
}
