<?php

require_once "RestBaseTest.php";

class HostIdTest extends RestBaseTest
{

    /**
     * Checks for valid json response
     */
    public function testHostID()
    {
        try
        {
            $jsonArray = $this->getResults("/host/$this->hostA");
            $this->assertEquals("hostA", $jsonArray[0]["name"]);
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
            $jsonArray = $this->getResults("host/$this->hostA");
        }
        catch (Pest_NotFound $e)
        {
            return;
        }
        $this->fail('Found resource');
    }


}
