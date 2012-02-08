<?php

require_once "RestBaseTest.php";

class VariableTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAllVariable()
    {
        try
        {
            $jsonArray = $this->pest->get('/variable');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test variable with host key 
     */
    public function testVariableWithHostKey()
    {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/variable?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if ($hostKey !== $data['hostkey'])
            {
                $this->fail("No hostkey found in data, Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test variable with name 
     */
    public function testVariableWithName()
    {

        $handle = "stop_signal";
        $jsonArray = $this->pest->get('/variable?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['name'] !== "$handle")
            {
                $this->fail("different name found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test software with scope 
     */
    public function testVariableWithScope()
    {

        $scope = "update_bins";
        $jsonArray = $this->pest->get('/variable?scope=' . $scope);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    /**
     * Test software with value 
     */
    public function testVariableWithValue()
    {

        $value = "term";
        $jsonArray = $this->pest->get('/variable?value=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    /**
     * Test software with type 
     */
    public function testVariableWithType()
    {

        $value = "string";
        $jsonArray = $this->pest->get('/variable?type=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

}
