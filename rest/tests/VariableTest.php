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
            $jsonArray = $this->getResults('/variable');
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
    public function testWithHostKey()
    {
        $jsonArray = $this->getResults('/variable?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if ($this->hostA !== $data['hostkey'])
            {
                $this->fail("No hostkey found in data, Expected :: " . $this->hostA);
            }
        }
    }

    /**
     * Test variable with name 
     */
    public function testWithName()
    {

        $handle = "dirs";
        $jsonArray = $this->getResults('/variable?name=' . $handle);
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
    public function testWithScope()
    {

        $scope = "control_runagent";
        $jsonArray = $this->getResults('/variable?scope=' . $scope);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    /**
     * Test software with value 
     */
    public function testWithValue()
    {

        $value = "true";
        $jsonArray = $this->getResults('/variable?value=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    /**
     * Test software with type 
     */
    public function testVariableWithType()
    {

        $value = "string";
        $jsonArray = $this->getResults('/variable?type=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

}
