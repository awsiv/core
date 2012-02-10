<?php

require_once "RestBaseTest.php";

class SetuidTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAllSetUid()
    {
        try
        {
            $jsonArray = $this->pest->get('/setuid');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test setuid with host key 
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->pest->get('/setuid?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if (!in_array($this->hostA, $data['hostkeys']))
            {
                $this->fail("No hostkey found in data, Expected :: " . $this->hostA);
            }
        }
    }

    /**
     * Test setuid with name 
     */
    public function testWithName()
    {

        $handle = "/usr/bin/chfn";
        $jsonArray = $this->pest->get('/setuid?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['path'] !== "$handle")
            {
                $this->fail("different path found in data, found :: " . $data['path'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test setuid with context 
     */
    public function testWithContext()
    {

        $context = "10_0_0_150";
        $jsonArray = $this->pest->get('/setuid?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if (!in_array($this->hostA, $data['hostkeys']))
            {
                $this->fail("No hostkey found in data, Expected :: " . $this->hostA);
            }
        }
    }

}
