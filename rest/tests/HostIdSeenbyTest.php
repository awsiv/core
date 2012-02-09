<?php

require_once "RestBaseTest.php";

class HostIdSeenbyTest extends RestBaseTest
{

    /**
     * Test if the returned data is correct 
     */
    public function testhostIdSeenBy()
    {
        try
        {
            $id = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
            $jsonArray = $this->pest->get("/host/$id/seen-by");
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test for the invalid key 
     */
    public function testhostIdSeenByWithInvalidKey()
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
                "hostkey" => "",
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
