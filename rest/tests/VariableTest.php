<?php

require_once "RestBaseTest.php";

class VariableTest extends RestBaseTest
{

    public function testAllVariable()
    {
        try
        {
            $jsonArray = $this->getResults('/variable');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(13, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

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

    public function testWithScope()
    {

        $scope = "control_runagent";
        $jsonArray = $this->getResults('/variable?scope=' . $scope);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    public function testWithValue()
    {

        $value = "true";
        $jsonArray = $this->getResults('/variable?value=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    public function testVariableWithType()
    {

        $value = "string";
        $jsonArray = $this->getResults('/variable?type=' . $value);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
    }

    public function testHostByContext()
    {
        try
        {
            $jsonArray = $this->getResults('/variable?context=PK_SHA_30565.*' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(6, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
